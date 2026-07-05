#pragma once

// Работает только на Windows. Требует линковки:
//   PortableDeviceGuids.lib
// и системных заголовков Windows SDK: PortableDeviceApi.h, PortableDevice.h

#ifdef _WIN32

#include <string>
#include <vector>
#include <functional>
#include <atlbase.h>
#include <PortableDeviceApi.h>
#include <PortableDevice.h>

// Аналог filesystem::directory_entry, но для объекта на MTP-устройстве.
struct mtpFileEntry {
    std::wstring objectId;      // ID объекта внутри устройства (не путь!)
    std::wstring name;          // Имя файла, как оно выглядит на устройстве
    std::wstring extension;     // Расширение в нижнем регистре, с точкой (".jpg")
    unsigned long long size = 0;
};

// Разобранный "виртуальный путь" вида mtp://<deviceId>/[objectId]
// Аналог того, что resolveVirtualPath извлекает из строки "mtp://host/..."
struct mtpAddress {
    std::wstring deviceId;
    std::wstring startObjectId;   // пусто = корень устройства (DEVICE_OBJECT_ID)
};

// Разбирает строку вида "mtp://<deviceId>/<objectId=...>" в mtpAddress.
// Бросает std::invalid_argument, если строка не в формате mtp://
mtpAddress parseMtpPath(const std::string &path);

// Основной класс-источник: заменяет собой связку
// filesystem::recursive_directory_iterator + filesystem::copy_file
// для случая, когда source или target — MTP-устройство.
class mtpWin {
public:
    explicit mtpWin(const mtpAddress &address);
    ~mtpWin();

    mtpWin(const mtpWin&) = delete;
    mtpWin& operator=(const mtpWin&) = delete;

    // Рекурсивно обходит устройство начиная с startObjectId (или корня)
    // и вызывает callback для каждого найденного файла.
    void forEachFile(const std::function<void(const mtpFileEntry&)> &callback) const;

    // Копирует файл с устройства в обычную папку на диске (target — реальный путь).
    // Возвращает false, если файл пропущен (уже существует и т.п.), не бросает исключений
    // на ожидаемых сценариях — только на реальных ошибках COM.
    bool copyToLocal(const mtpFileEntry &entry, const std::wstring &targetDir) const;

    bool isValid() const { return m_content_ != nullptr; }

private:
    CComPtr<IPortableDevice> m_device_;
    CComPtr<IPortableDeviceContent> m_content_;
    std::wstring m_rootObjectId_;

    void enumerateRecursive(const std::wstring &objectId,
                             const std::function<void(const mtpFileEntry&)> &callback) const;
};

// Список подключённых MTP-устройств — аналог "какие точки монтирования сейчас есть в gvfs"
struct mtpDeviceInfo {
    std::wstring id;
    std::wstring friendlyName;
};
std::vector<mtpDeviceInfo> listConnectedMtpDevices();

#endif // _WIN32