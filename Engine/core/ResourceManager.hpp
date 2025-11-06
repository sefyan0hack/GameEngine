#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

class ResourceManager
{
private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    class Proxy {
        public:
            Proxy(const std::string& key) : m_manager(ResourceManager::get_instance()), m_key(key){}
        
            template <class T>
            void operator=(T&& obj) {
                m_manager.store(m_key, std::forward<T>(obj));
            }
        
            template <class T>
            operator std::shared_ptr<T>() const {
                return m_manager.get<T>(m_key);
            }
        
            template <class T>
            std::shared_ptr<T> get() const {
                return m_manager.get<T>(m_key);
            }
        
        private:
            ResourceManager& m_manager;
            std::string m_key;
        };
public:
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    auto operator[](const std::string& key) -> Proxy
    {
        return Proxy(key);
    }

    template <class T>
    auto get(const std::string &key) -> std::shared_ptr<T>
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return std::static_pointer_cast<T>(m_Resources.at(key));
    }

    template <class T>
    void store(const std::string& key, T&& resource)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Resources[key] = std::make_shared<std::remove_reference_t<T>>(std::forward<T>(resource));
    }

    template <class T>
    void store(const std::string& key, std::shared_ptr<T> resource)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Resources[key] = std::move(resource);
    }

    bool exists(const std::string& key) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_Resources.find(key) != m_Resources.end();
    }

    void remove(const std::string& key) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Resources.erase(key);
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_Resources.clear();
    }

    static auto get_instance() -> ResourceManager&
    {
        static ResourceManager S_instance;
        return S_instance;
    }

private:
    std::mutex m_mutex;
    std::unordered_map<std::string, std::shared_ptr<void>> m_Resources;

};

// inline static ResourceManager& ResManager = ResourceManager::get_instance();

#define ResManager ResourceManager::get_instance()