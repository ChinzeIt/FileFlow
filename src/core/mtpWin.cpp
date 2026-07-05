#ifdef _WIN32

#include "mtpWin.hpp"

#include <fstream>
#include <stdexcept>

namespace {

std::wstring toWide(const std::string &s) {
    if (s.empty()) return {};
        int needed = MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), nullptr, 0);
        std::wstring result(needed, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), result.data(), needed);
        return result;
}

std::wstring toLowerW(std::wstring s) {
    for (auto &c : s) c = towlower(c);
    return s;
}

// Клиентские данные, которые Windows требует передавать при открытии устройства.
CComPtr<IPortableDeviceValues> makeClientInfo() {
    CComPtr<IPortableDeviceValues> values;
    CoCreateInstance(CLSID_PortableDeviceValues, nullptr, CLSCTX_INPROC_SERVER,
                      IID_PPV_ARGS(&values));
    values->SetStringValue(WPD_CLIENT_NAME, L"FileFlow");
    values->SetUnsignedIntegerValue(WPD_CLIENT_MAJOR_VERSION, 1);
    values->SetUnsignedIntegerValue(WPD_CLIENT_MINOR_VERSION, 0);
    values->SetUnsignedIntegerValue(WPD_CLIENT_SECURITY_QUALITY_OF_SERVICE, SECURITY_IMPERSONATION);
    return values;
}

} // namespace

mtpAddress parseMtpPath(const std::string &path) {
    const std::string prefix = "mtp://";
    if (path.find(prefix) != 0)
        throw std::invalid_argument("Not an mtp:// path");

    std::string rest = path.substr(prefix.size());
    auto slash = rest.find('/');

    mtpAddress addr;
    addr.deviceId = toWide(slash == std::string::npos ? rest : rest.substr(0, slash));
    addr.startObjectId = slash == std::string::npos
        ? L""
        : toWide(rest.substr(slash + 1));
    return addr;
}

std::vector<mtpDeviceInfo> listConnectedMtpDevices() {
    std::vector<mtpDeviceInfo> result;

    CComPtr<IPortableDeviceManager> manager;
    if (FAILED(CoCreateInstance(CLSID_PortableDeviceManager, nullptr,
                                 CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&manager))))
        return result;

    DWORD count = 0;
    manager->GetDevices(nullptr, &count);
    if (count == 0) return result;

    std::vector<LPWSTR> ids(count);
    if (FAILED(manager->GetDevices(ids.data(), &count)))
        return result;

    for (DWORD i = 0; i < count; ++i) {
        DWORD nameLen = 0;
        manager->GetDeviceFriendlyName(ids[i], nullptr, &nameLen);
        std::wstring name(nameLen, L'\0');
        if (nameLen > 0)
            manager->GetDeviceFriendlyName(ids[i], name.data(), &nameLen);

        result.push_back({ ids[i], name });
        CoTaskMemFree(ids[i]);
    }
    return result;
}

mtpWin::mtpWin(const mtpAddress &address) {
    if (FAILED(CoCreateInstance(CLSID_PortableDeviceFTM, nullptr,
                                 CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_device_))))
        return;

    auto clientInfo = makeClientInfo();
    if (FAILED(m_device_->Open(address.deviceId.c_str(), clientInfo))) {
        m_device_.Release();
        return;
    }

    if (FAILED(m_device_->Content(&m_content_))) {
        m_content_.Release();
        m_device_.Release();
        return;
    }

    m_rootObjectId_ = address.startObjectId.empty()
        ? std::wstring(WPD_DEVICE_OBJECT_ID)
        : address.startObjectId;
}

mtpWin::~mtpWin() {
    if (m_device_) m_device_->Close();
}

void mtpWin::forEachFile(const std::function<void(const mtpFileEntry&)> &callback) const {
    if (!isValid()) return;
    enumerateRecursive(m_rootObjectId_, callback);
}

void mtpWin::enumerateRecursive(const std::wstring &objectId,
                                    const std::function<void(const mtpFileEntry&)> &callback) const {
    CComPtr<IEnumPortableDeviceObjectIDs> enumIds;
    if (FAILED(m_content_->EnumObjects(0, objectId.c_str(), nullptr, &enumIds)))
        return;

    CComPtr<IPortableDeviceProperties> properties;
    m_content_->Properties(&properties);

    LPWSTR childIds[16];
    ULONG fetched = 0;

    while (SUCCEEDED(enumIds->Next(16, childIds, &fetched)) && fetched > 0) {
        for (ULONG i = 0; i < fetched; ++i) {
            std::wstring childId = childIds[i];

            CComPtr<IPortableDeviceValues> values;
            if (SUCCEEDED(properties->GetValues(childId.c_str(), nullptr, &values))) {
                LPWSTR name = nullptr;
                GUID contentType = GUID_NULL;
                ULONGLONG size = 0;

                values->GetStringValue(WPD_OBJECT_ORIGINAL_FILE_NAME, &name);
                values->GetGuidValue(WPD_OBJECT_CONTENT_TYPE, &contentType);
                values->GetUnsignedLargeIntegerValue(WPD_OBJECT_SIZE, &size);

                bool isFolder = (contentType == WPD_CONTENT_TYPE_FOLDER
                                  || contentType == WPD_CONTENT_TYPE_FUNCTIONAL_OBJECT);

                if (isFolder) {
                    enumerateRecursive(childId, callback);
                } else if (name) {
                    std::wstring wname(name);
                    auto dot = wname.find_last_of(L'.');
                    mtpFileEntry entry;
                    entry.objectId = childId;
                    entry.name = wname;
                    entry.extension = dot == std::wstring::npos ? L"" : toLowerW(wname.substr(dot));
                    entry.size = size;
                    callback(entry);
                }
                if (name) CoTaskMemFree(name);
            }
        }
    }
}

bool mtpWin::copyToLocal(const mtpFileEntry &entry, const std::wstring &targetDir) const {
    if (!isValid()) return false;

    CComPtr<IPortableDeviceResources> resources;
    if (FAILED(m_content_->Transfer(&resources)))
        return false;

    CComPtr<IStream> stream;
    DWORD optimalBufferSize = 0;
    if (FAILED(resources->GetStream(entry.objectId.c_str(), WPD_RESOURCE_DEFAULT,
                                     STGM_READ, &optimalBufferSize, &stream)))
        return false;

    std::wstring destPath = targetDir;
    if (!destPath.empty() && destPath.back() != L'\\')
        destPath += L'\\';
    destPath += entry.name;

    std::ofstream out(destPath, std::ios::binary);
    if (!out.is_open())
        return false;

    std::vector<char> buffer(optimalBufferSize > 0 ? optimalBufferSize : 65536);
    ULONG bytesRead = 0;
    HRESULT hr = S_OK;

    do {
        hr = stream->Read(buffer.data(), static_cast<ULONG>(buffer.size()), &bytesRead);
        if (bytesRead > 0)
            out.write(buffer.data(), bytesRead);
    } while (hr == S_OK && bytesRead > 0);

    return SUCCEEDED(hr);
}

#endif // _WIN32