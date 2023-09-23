#include "variant.hpp"
#include <type_traits>

namespace Variant
{
	// Load value from file into a Variant
	template <class T>
	Variant LoadFromFile(std::istream& f)
	{
		Variant v;
		if constexpr (std::is_same_v<T, std::string>)
		{
			uint32_t len = 0;
			f.read(reinterpret_cast<char*>(&len), sizeof(len));
			std::string value(len, '\0');
			f.read(&value[0], len);
			v.emplace<std::string>(std::move(value));
		}
		else
		{
			T value{};
			f.read(reinterpret_cast<char*>(&value), sizeof(value));
			v.emplace<T>(value);
		}
		return v;
	}

	// Save value from file into a Variant
	template <class T>
	void SaveToFile(std::ostream& f, Variant& v)
	{
		// Special case for strings
		if constexpr (std::is_same_v<T, std::string>)
		{
			std::string value = std::get<std::string>(v);
			uint32_t len = static_cast<uint32_t>(value.size());
			f.write(reinterpret_cast<char*>(&len), sizeof(uint32_t));
			f.write(value.data(), len);
		}
		else
			f.write(reinterpret_cast<char*>(&std::get<T>(v)), sizeof(T));
	}

	// Load a VariantDB from path
	VariantDB LoadVariantDB(std::istream& f)
	{
		// The version number should always be 1
		uint32_t version = 0;
		f.read(reinterpret_cast<char*>(&version), sizeof(version));
		if (version != 1)
			return {};
		// Parse file
		VariantDB db;
		while (!f.eof())
		{
			Variant v;
			// If the type is 0, it's a file end marker
			uint32_t type = VariantUnused;
			f.read(reinterpret_cast<char*>(&type), sizeof(type));
			if (type == VariantUnused)
				break;
			// Read key length & the key itself
			uint32_t key_len = 0;
			f.read(reinterpret_cast<char*>(&key_len), sizeof(key_len));
			std::string key(key_len, '\0');
			f.read(&key[0], key_len);
			// Read the data depending on the type
			switch (static_cast<Type>(type))
			{
			case VariantFloat:
				v = LoadFromFile<float>(f);
				break;
			case VariantString:
				v = LoadFromFile<std::string>(f);
				break;
			case VariantVec2f:
				v = LoadFromFile<Vec2f>(f);
				break;
			case VariantVec3f:
				v = LoadFromFile<Vec3f>(f);
				break;
			case VariantUint:
				v = LoadFromFile<uint32_t>(f);
				break;
			case VariantRectf:
				v = LoadFromFile<Rectf>(f);
				break;
			case VariantInt:
				v = LoadFromFile<int32_t>(f);
				break;
			default:
				break;
			}
			db.push_back(std::make_pair(key, v));
		}
		return db;
	}

	// Save VariantDB to path
	bool SaveVariantDB(std::ostream& f, VariantDB& db)
	{
		uint32_t version = 1;
		f.write(reinterpret_cast<char*>(&version), sizeof(version));
		for (auto& [k, v] : db)
		{
			uint32_t type = static_cast<uint32_t>(v.index());
			f.write(reinterpret_cast<char*>(&type), sizeof(uint32_t));
			uint32_t key_len = static_cast<uint32_t>(k.size());
			f.write(reinterpret_cast<char*>(&key_len), sizeof(uint32_t));
			f.write(k.data(), key_len);
			switch (type)
			{
			case VariantUnused:
				break;
			case VariantFloat:
				SaveToFile<float>(f, v);
				break;
			case VariantString:
				SaveToFile<std::string>(f, v);
				break;
			case VariantVec2f:
				SaveToFile<Vec2f>(f, v);
				break;
			case VariantVec3f:
				SaveToFile<Vec3f>(f, v);
				break;
			case VariantUint:
				SaveToFile<uint32_t>(f, v);
				break;
			case VariantRectf:
				SaveToFile<Rectf>(f, v);
				break;
			case VariantInt:
				SaveToFile<int32_t>(f, v);
				break;
			default: // Something is wrong, abort saving
				return false;
				break;
			}
		}
		uint32_t end = 0;
		f.write(reinterpret_cast<char*>(&end), sizeof(end));
		return true;
	}
};