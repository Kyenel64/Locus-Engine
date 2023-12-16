#include "Lpch.h"
#include "ConsolePanel.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

#include "Locus/Scripting/ScriptEngine.h"
#include "Locus/ImGui/ImGuiLayer.h"

namespace Locus
{
	// From imgui_demo.cpp
	struct ConsoleLog
	{
		ImGuiTextBuffer     Buf;
		ImGuiTextFilter     Filter;
		ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
		bool                AutoScroll;  // Keep scrolling if already at the bottom.

		ConsoleLog()
		{
			AutoScroll = true;
			Clear();
		}

		void    Clear()
		{
			Buf.clear();
			LineOffsets.clear();
			LineOffsets.push_back(0);
		}

		void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
		{
			int old_size = Buf.size();
			va_list args;
			va_start(args, fmt);
			Buf.appendfv(fmt, args);
			va_end(args);
			for (int new_size = Buf.size(); old_size < new_size; old_size++)
				if (Buf[old_size] == '\n')
					LineOffsets.push_back(old_size + 1);
		}

		void    Draw(const char* title, bool* p_open = NULL)
		{
			if (!ImGui::Begin(title, p_open))
			{
				ImGui::End();
				return;
			}

			// Options menu
			if (ImGui::BeginPopup("Options"))
			{
				ImGui::Checkbox("Auto-scroll", &AutoScroll);
				ImGui::EndPopup();
			}

			// Main window
			if (ImGui::Button("Options"))
				ImGui::OpenPopup("Options");
			ImGui::SameLine();
			bool clear = ImGui::Button("Clear");
			ImGui::SameLine();
			bool copy = ImGui::Button("Copy");
			ImGui::SameLine();
			Filter.Draw("Filter", -100.0f);

			ImGui::Separator();

			if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
			{
				if (clear)
					Clear();
				if (copy)
					ImGui::LogToClipboard();

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				const char* buf = Buf.begin();
				const char* buf_end = Buf.end();
				if (Filter.IsActive())
				{
					// In this example we don't use the clipper when Filter is enabled.
					// This is because we don't have random access to the result of our filter.
					// A real application processing logs with ten of thousands of entries may want to store the result of
					// search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
					for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
					{
						const char* line_start = buf + LineOffsets[line_no];
						const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
						ImGui::PushStyleColor(ImGuiCol_Text, LocusColors::Pink);
						if (Filter.PassFilter(line_start, line_end))
							ImGui::TextUnformatted(line_start, line_end);
						ImGui::PopStyleColor();
					}
				}
				else
				{
					// The simplest and easy way to display the entire buffer:
					//   ImGui::TextUnformatted(buf_begin, buf_end);
					// And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
					// to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
					// within the visible area.
					// If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
					// on your side is recommended. Using ImGuiListClipper requires
					// - A) random access into your data
					// - B) items all being the  same height,
					// both of which we can handle since we have an array pointing to the beginning of each line of text.
					// When using the filter (in the block of code above) we don't have random access into the data to display
					// anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
					// it possible (and would be recommended if you want to search through tens of thousands of entries).
					ImGuiListClipper clipper;
					clipper.Begin(LineOffsets.Size);
					while (clipper.Step())
					{
						for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
						{
							const char* line_start = buf + LineOffsets[line_no];
							const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
							ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 1.0f });
							ImGui::TextUnformatted(line_start, line_end);
							ImGui::PopStyleColor();
						}
					}
					clipper.End();
				}
				ImGui::PopStyleVar();

				// Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
				// Using a scrollbar or mouse-wheel will take away from the bottom edge.
				if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
					ImGui::SetScrollHereY(1.0f);
			}
			ImGui::EndChild();
			ImGui::End();
		}
	};

	ConsolePanel::ConsolePanel()
	{

	}

	void ConsolePanel::OnImGuiRender()
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_TabBarAlignLeft | ImGuiWindowFlags_DockedWindowBorder;
		ImGui::Begin("Console", false, windowFlags);

		static ConsoleLog log;

		std::queue<ExceptionData>& exceptions = ScriptEngine::GetExceptions();
		while (!exceptions.empty())
		{
			ExceptionData& data = exceptions.front();
			log.AddLog("[12:39:59] %s - %s\n%s\n---\n", data.Type.c_str(), data.Message.c_str(), data.Trace.c_str());
			exceptions.pop();
		}
		
		log.Draw("Console");

		ImGui::End();
	}
}