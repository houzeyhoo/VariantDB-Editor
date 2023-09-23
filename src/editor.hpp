#pragma once
#include "variant.hpp"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Internally manages a Variant and provides an interface to edit it.
class VariantEditor
{
	std::string name;
	Variant::Variant variant;
public:
	VariantEditor() {};
	VariantEditor(std::string& name, Variant::Variant variant)
		: name(name), variant(variant) {};
	// Gives variant a new type and sets default value.
	void ResetVariant(Variant::Type newType);
	const std::string& GetName();
	const Variant::Variant& GetVariant();
	// Returns false if the variant delete button was clicked, true otherwise.
	bool Show(bool readOnly);
};

class VariantDBEditor
{
	HWND wnd;
	std::vector<VariantEditor> editors;
	std::wstring filePath;
	std::string searchString;
	bool readOnly = true;

	void Reset();
	void LoadFile(const std::wstring& path);
	void SaveFile(const std::wstring& path);
	std::string GetTitle();
public:
	VariantDBEditor(HWND parent) : wnd(parent) {};
	void Show(int windowFlags);
};