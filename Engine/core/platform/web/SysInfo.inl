

auto os::host::name_tag() -> os::Target
{
    return os::Target::Web;
}
    
auto os::host::name() -> std::string
{
    return "web";
}

auto os::host::arch_tag() -> os::Arch
{
    return os::build::arch_tag();
}

auto os::host::arch() -> std::string
{
    return os::build::arch();
}