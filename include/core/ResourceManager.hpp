#pragma once

class ResourceManager
{
private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    class Proxy {
    public:
        Proxy(const std::string_view& key) : m_key(key) {}

        template <class T>
        void operator=(T&& obj) {
            ResourceManager::getInstance().m_Resources[m_key] = std::make_shared<std::remove_reference_t<T>>(std::forward<T>(obj));
        }

        template <class T>
        operator std::shared_ptr<T>() const {
            return ResourceManager::getInstance().get<T>(m_key);
        }

        template <class T>
        std::shared_ptr<T> get() const {
            return ResourceManager::getInstance().get<T>(m_key);
        }

    private:
        std::string_view m_key;
    };
public:
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    auto operator[](const std::string_view& key) -> Proxy
    {
        return Proxy(key);
    }

    template <class T>
    auto get(const std::string_view &path) -> std::shared_ptr<T>
    {
        return std::static_pointer_cast<T>(m_Resources.at(path));
    }

    static auto getInstance() -> ResourceManager&
    {
        static ResourceManager S_instance;
        return S_instance;
    }

private:

    std::unordered_map<std::string_view, std::shared_ptr<void>> m_Resources;

    FOR_TEST
};

inline static ResourceManager& ResManager = ResourceManager::getInstance();