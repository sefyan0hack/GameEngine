

auto sys::host::name() -> sys::Target
{
    return sys::Target::Web;
}
    
auto sys::host::name_str() -> std::string
{
    return "web";
}

auto sys::host::arch() -> sys::Arch
{
    return sys::build::arch();
}

auto sys::host::arch_str() -> std::string
{
    return sys::build::arch_str();
}