

auto sys::host::name_tag() -> sys::Target
{
    return sys::Target::Web;
}
    
auto sys::host::name() -> std::string
{
    return "web";
}

auto sys::host::arch_tag() -> sys::Arch
{
    return sys::build::arch_tag();
}

auto sys::host::arch() -> std::string
{
    return sys::build::arch();
}