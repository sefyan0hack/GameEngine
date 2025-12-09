#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <engine_export.h>
#include <utility>

class ENGINE_EXPORT ResMan
{
private:
    class Proxy {
        public:
            Proxy(const std::string& key) : m_manager(ResMan::self()), m_key(key){}
        
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
            ResMan& m_manager;
            std::string m_key;
    };
public:
    ResMan() = default;
    ResMan(const ResMan&) = delete;
    ResMan& operator=(const ResMan&) = delete;

    static auto RES(const std::string& key) -> Proxy
    {
        return Proxy(key);
    }

    template <class T>
    auto get(const std::string &key) -> std::shared_ptr<T>
    {
        return std::static_pointer_cast<T>(m_Resources.at(key));
    }

    template <class T>
    void store(const std::string& key, T&& resource)
    {
        m_Resources[key] = std::make_shared<std::remove_reference_t<T>>(std::forward<T>(resource));
    }

    template <class T>
    void store(const std::string& key, std::shared_ptr<T> resource)
    {
        m_Resources[key] = std::move(resource);
    }

    bool exists(const std::string& key) {
        return m_Resources.find(key) != m_Resources.end();
    }

    void remove(const std::string& key) {
        m_Resources.erase(key);
    }

    void clear() {
        m_Resources.clear();
    }

    static auto self() -> ResMan&
    {
        static ResMan ins;
        return ins;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<void>> m_Resources;

};
