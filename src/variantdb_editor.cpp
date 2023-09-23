#include "editor.hpp"
#include "util.hpp"
#include <imgui.h>
#include <imgui_stdlib.h>

void VariantDBEditor::Reset()
{
	filePath.clear();
	editors.clear();
	searchString.clear();
	readOnly = true;
}

void VariantDBEditor::LoadFile(const std::wstring& path)
{
	std::ifstream f(path, std::ios::binary);
	if (f.is_open())
	{
		Reset();
		auto db = Variant::LoadVariantDB(f);
		for (auto& [k, v] : db)
			editors.emplace_back(k, v);
		filePath = path;
		f.close();
	}
	else
		ErrorMessageBox("Failed to open file for reading");
}

void VariantDBEditor::SaveFile(const std::wstring& path)
{
	std::ofstream f(path, std::ios::binary);
	if (f.is_open())
	{
		Variant::VariantDB db;
		for (auto& editor : editors)
			db.emplace_back(editor.GetName(), editor.GetVariant());
		if (!Variant::SaveVariantDB(f, db))
			ErrorMessageBox("Failed to save VariantDB (data is corrupt)");
		filePath = path;
		f.close();
	}
	else
		ErrorMessageBox("Failed to open file for writing");
}

std::string VariantDBEditor::GetTitle()
{
	std::string title = "VariantDB Editor";
	if (!filePath.empty())
		title += " - " + WStringToString(filePath);
	return title;
}

void VariantDBEditor::Show(int windowFlags)
{
	ImGui::Begin((GetTitle() + "###VDBEditor").data(), nullptr, windowFlags);
	if (ImGui::BeginMenuBar())
	{
		// File menu - new file, open, save, etc.
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
				Reset();
			if (ImGui::MenuItem("Reload"))
			{
				std::wstring tmp = filePath;
				LoadFile(tmp);
			}
			if (ImGui::MenuItem("Open"))
			{
				auto path = ShowOpenFileDialog(wnd).value_or(L"");
				if (!path.empty())
					LoadFile(path);
			}
			if (ImGui::MenuItem("Save"))
			{
				if (filePath.empty())
				{
					auto path = ShowSaveFileDialog(wnd).value_or(L"");
					if (!path.empty())
						SaveFile(path);
				}
				else
					SaveFile(filePath);
			}
			if (ImGui::MenuItem("Save As..."))
			{
				auto path = ShowSaveFileDialog(wnd).value_or(L"");
				if (!path.empty())
					SaveFile(path);
			}
			ImGui::EndMenu();
		}

		// Edit menu - create new variant, maybe more stuff later?
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Add Variant"))
				editors.emplace(editors.begin());
			ImGui::EndMenu();
		}

		// Search bar
		ImGui::Separator();
		ImGui::Text("Search:");
		ImGui::SetNextItemWidth(-150);
		ImGui::InputText("##search", &searchString);
		ImGui::Separator();
		ImGui::Checkbox("Read-Only", &readOnly);
		ImGui::EndMenuBar();
	}

	// Now, for the actual variants...
	ImGui::BeginChild("##variants", ImVec2(0, 0), true);
	int id = 0;
	for (auto it = editors.begin(); it != editors.end();)
	{
		// Search filter
		auto searchLower = GetLowercaseString(searchString);
		auto nameLower = GetLowercaseString(it->GetName());
		if (nameLower.find(searchLower) == std::string::npos)
		{
			it++;
			continue;
		}
		ImGui::PushID(id++);
		// If the editor returns false, the delete button was clicked
		if (!it->Show(readOnly))
			it = editors.erase(it);
		else
			it++;
		ImGui::PopID();
	}
	ImGui::EndChild();
	ImGui::End();
}