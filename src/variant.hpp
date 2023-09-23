#pragma once
#include <fstream>
#include <string>
#include <variant>
#include <vector>

namespace Variant
{
	enum Type
	{
		VariantUnused = 0,
		VariantFloat,
		VariantString,
		VariantVec2f,
		VariantVec3f,
		VariantUint,
		VariantEntity, // Unused
		VariantComponent, // Unused
		VariantRectf,
		VariantInt,
	};
	// Dummy types
	struct Unused {};
	struct Entity {};
	struct Component {};
	struct Vec2f { float x, y; };
	struct Vec3f { float x, y, z; };
	struct Rectf { float x, y, w, h; };

	// The variant types are padded with dummy structs to make sure
	// its internal index will match VariantDB file format type indices
	using Variant = std::variant<
		Unused, float, std::string, Vec2f, Vec3f,
		uint32_t, Entity, Component, Rectf, int32_t
	>;
	// Use vector instead of map to make sure the order is preserved
	using VariantDB = std::vector<std::pair<std::string, Variant>>;

	// Load value from file into a Variant
	template <class T>
	Variant LoadFromFile(std::istream& f);

	// Save value from file into a Variant
	template <class T>
	void SaveToFile(std::ostream& f, Variant& v);

	// Load a VariantDB from path
	VariantDB LoadVariantDB(std::istream& f);

	// Save VariantDB to path
	bool SaveVariantDB(std::ostream& f, VariantDB& db);
};