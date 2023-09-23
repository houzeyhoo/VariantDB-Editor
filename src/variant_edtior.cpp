#include "editor.hpp"
#include "util.hpp"
#include <imgui.h>
#include <imgui_stdlib.h>

// Empty strings are used for types that are not supported (Entity, Component)
constexpr const char* typeNames[] = {
	"Unused", "Float", "String", "Vec2f", "Vec3f", "UInt", "", "", "Rectf", "Int"
};

// Some stuff used by Variant input fields
constexpr float floatSteps[] = { 1.0f, 5.0f };
constexpr int intSteps[] = { 1, 5 };

// Getters
const std::string& VariantEditor::GetName() { return name; }
const Variant::Variant& VariantEditor::GetVariant() { return variant; }

void VariantEditor::ResetVariant(Variant::Type newType)
{
	switch (newType)
	{
	case Variant::VariantUnused: variant = Variant::Unused(); break;
	case Variant::VariantFloat: variant = 0.0f; break;
	case Variant::VariantString: variant = ""; break;
	case Variant::VariantVec2f: variant = Variant::Vec2f(); break;
	case Variant::VariantVec3f: variant = Variant::Vec3f(); break;
	case Variant::VariantUint: variant = 0U; break;
	case Variant::VariantRectf: variant = Variant::Rectf(); break;
	case Variant::VariantInt: variant = 0; break;
	}
}

bool VariantEditor::Show(bool readOnly)
{
	// Disable picker & button if readOnly
	if (readOnly)
		ImGui::BeginDisabled();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));
	// Variant delete button
	bool exists = true;
	if (ImGui::Button("x##delete"))
		exists = false;
	ImGui::SameLine();

	// Variant type picker
	size_t selected = variant.index();
	ImGui::SetNextItemWidth(std::min(ImGui::GetWindowWidth() * 0.2f, 80.0f));
	if (ImGui::BeginCombo("##type", typeNames[selected]))
	{
		for (int i = 0; i < IM_ARRAYSIZE(typeNames); i++)
		{
			// Skip the empty strings
			if (typeNames[i][0] == '\0')
				continue;
			if (ImGui::Selectable(typeNames[i], i == selected))
			{
				if (i == selected)
					continue;
				ResetVariant(static_cast<Variant::Type>(i));
			}
		}
		ImGui::EndCombo();
	}
	ImGui::SameLine();

	if (readOnly)
		ImGui::EndDisabled();
	// If readOnly, make data read only (but can copy)
	int inputFlags = readOnly ? ImGuiInputTextFlags_ReadOnly : 0;

	// Display name
	ImGui::SetNextItemWidth(std::min(ImGui::GetWindowWidth() * 0.4f, 160.0f));
	ImGui::InputText("##name", &name);
	ImGui::SameLine();

	// Display variant data
	ImGui::SetNextItemWidth(-FLT_MIN);
	switch (variant.index())
	{
	case Variant::VariantUnused:
		ImGui::BeginDisabled();
		ImGui::InputText("##data", const_cast<char*>("<unused type>"), 1, ImGuiInputTextFlags_ReadOnly);
		ImGui::EndDisabled();
		break;
	case Variant::VariantFloat:
		ImGui::InputScalar("##data", ImGuiDataType_Float, &std::get<float>(variant), &floatSteps[0], &floatSteps[1], 0, inputFlags);
		break;
	case Variant::VariantString:
		if (IsStringPrintable(std::get<std::string>(variant)))
			ImGui::InputText("##data", &std::get<std::string>(variant), inputFlags);
		else
		{
			ImGui::BeginDisabled();
			ImGui::InputText("##data", const_cast<char*>("<contains unprintable characters>"), ImGuiInputTextFlags_ReadOnly);
			ImGui::EndDisabled();
		}
		break;
	case Variant::VariantVec2f:
		ImGui::InputScalarN("##data", ImGuiDataType_Float, &std::get<Variant::Vec2f>(variant), 2, &floatSteps[0], &floatSteps[1], 0, inputFlags);
		break;
	case Variant::VariantVec3f:
		ImGui::InputScalarN("##data", ImGuiDataType_Float, &std::get<Variant::Vec3f>(variant), 3, &floatSteps[0], &floatSteps[1], 0, inputFlags);
		break;
	case Variant::VariantUint:
		ImGui::InputScalar("##data", ImGuiDataType_U32, &std::get<uint32_t>(variant), &intSteps[0], &intSteps[1], 0, inputFlags);
		break;
	case Variant::VariantRectf:
		ImGui::InputScalarN("##data", ImGuiDataType_Float, &std::get<Variant::Rectf>(variant), 4, &floatSteps[0], &floatSteps[1], 0, inputFlags);
		break;
	case Variant::VariantInt:
		ImGui::InputScalar("##data", ImGuiDataType_S32, &std::get<int32_t>(variant), &intSteps[0], &intSteps[1], 0, inputFlags);
		break;
	default:
		ImGui::Text("Unknown variant type");
		break;
	}

	ImGui::PopStyleVar();
	return exists;
}