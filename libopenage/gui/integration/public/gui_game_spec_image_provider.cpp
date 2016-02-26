// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "gui_game_spec_image_provider.h"

#include <cassert>

#include "../private/gui_game_spec_image_provider_by_filename_impl.h"
#include "../private/gui_game_spec_image_provider_by_graphic_id_impl.h"
#include "../private/gui_game_spec_image_provider_by_terrain_id_impl.h"

namespace openage {
namespace gui {

GuiGameSpecImageProvider::GuiGameSpecImageProvider(GuiEventQueue *render_updater, Type type)
	:
	GuiImageProvider{[render_updater, type] () -> std::unique_ptr<GuiImageProviderImpl> {
		switch(type) {
		case Type::ByFilename:
			return std::make_unique<GuiGameSpecImageProviderByFilenameImpl>(render_updater);
			break;

		case Type::ByGraphicId:
			return std::make_unique<GuiGameSpecImageProviderByGraphicIdImpl>(render_updater);
			break;

		case Type::ByTerrainId:
			return std::make_unique<GuiGameSpecImageProviderByTerrainIdImpl>(render_updater);
			break;
		}

		assert(false);
		return std::unique_ptr<GuiImageProviderImpl>{};
	}()} {
}

GuiGameSpecImageProvider::~GuiGameSpecImageProvider() {
}

}} // namespace openage::gui