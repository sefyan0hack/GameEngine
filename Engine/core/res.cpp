#include "res.hpp"
#include <engine_export.h>

CMRC_DECLARE(res);
ENGINE_EXPORT auto embed_filesystem = cmrc::res::get_filesystem();