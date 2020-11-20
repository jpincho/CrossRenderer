#pragma once
#include <string>
#include <vector>
#include <stdio.h>
#include "../CrossRenderer.h"

bool LoadFileContents ( const std::string &Path, std::vector <uint8_t> &Contents );
bool LoadTextureFile ( const std::string &Path, CrossRenderer::TextureHandle &TextureObject );
