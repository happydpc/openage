// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>

namespace openage {
namespace gui {

class GuiRenderer;
class GuiImageProvider;
class GuiEngineImpl;
struct GuiSingletonItemsInfo;

/**
 * Represents one QML execution environment.
 */
class GuiEngine {
public:
	explicit GuiEngine(GuiRenderer *renderer, const std::vector<GuiImageProvider*> &image_providers=std::vector<GuiImageProvider*>(), GuiSingletonItemsInfo *singleton_items_info=nullptr);
	~GuiEngine();

private:
	friend class GuiEngineImpl;
	std::unique_ptr<GuiEngineImpl> impl;
};

}} // namespace openage::gui