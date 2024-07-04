#include "messagesformaction.h"

#include <cstring>

#include "config.h"
#include "fmtstrformatter.h"
#include "keymap.h"
#include "logger.h"
#include "listformatter.h"
#include "strprintf.h"
#include "utils.h"
#include "view.h"

namespace newsboat {

MessagesFormAction::MessagesFormAction(View& vv,
	std::string formstr,
	ConfigContainer* cfg)
	: FormAction(vv, formstr, cfg)
	, quit(false)
	, apply_search(false)
	, listview("messages", FormAction::f, cfg->get_configvalue_as_int("scrolloff"))
{
}

bool MessagesFormAction::process_operation(Operation op,
	const std::vector<std::string>& /* args */,
	BindingType /*bindingType*/)
{
	bool hardquit = false;
	switch (op) {
	case OP_QUIT:
		quit = true;
		break;
	case OP_HARDQUIT:
		hardquit = true;
		break;
	case OP_SEARCH: {
		std::vector<QnaPair> qna;
		qna.push_back(QnaPair(_("Search for: "), ""));
		this->start_qna(qna, OP_INT_START_SEARCH, &searchhistory);
	}
	break;
	case OP_CLEARFILTER:
		apply_search = false;
		do_redraw = true;
		break;
	default:
		if (handle_list_operations(listview, op)) {
			break;
		}
		break;
	}
	if (hardquit) {
		while (v.formaction_stack_size() > 0) {
			v.pop_current_formaction();
		}
	} else if (quit) {
		v.pop_current_formaction();
	}
	return true;
}

void MessagesFormAction::prepare()
{
	if (do_redraw) {
		recalculate_widget_dimensions();

		const unsigned int width = listview.get_width();

		FmtStrFormatter fmt;
		fmt.register_fmt('N', PROGRAM_NAME);
		fmt.register_fmt('V', utils::program_version());
		fmt.register_fmt('T', title());
		set_title(fmt.do_format("%N %V - %T", width));

		std::vector<std::string> colors = utils::tokenize(
				cfg->get_configvalue("search-highlight-colors"), " ");
		set_value("highlight", make_colorstring(colors));
		ListFormatter listfmt;

		const auto should_be_visible = [&](const std::string& desc) {
			return !apply_search
				|| strcasestr(desc.c_str(), searchphrase.c_str()) != nullptr;
		};

		std::vector<std::string> lines;
		for (const auto& desc : logger::get_logs()) {
			auto line = std::string(desc);
			if (should_be_visible(line)) {
				lines.push_back(line);
			}
		}

		auto render_line = [this, lines](std::uint32_t index, std::uint32_t width) -> StflRichText {
			(void)width;
			auto line = lines[index];
			line = utils::quote_for_stfl(line);
			if (apply_search && searchphrase.length() > 0) {
				line = utils::replace_all(line, searchphrase, strprintf::fmt("<hl>%s</>", searchphrase));
			}
			return StflRichText::from_quoted(line);
		};
		listview.invalidate_list_content(lines.size(), render_line);

		do_redraw = false;
	}
	quit = false;
}

void MessagesFormAction::init()
{
	set_keymap_hints();
}

const std::vector<KeyMapHintEntry>& MessagesFormAction::get_keymap_hint() const
{
	static const std::vector<KeyMapHintEntry> hints = {{OP_QUIT, _("Quit")},
		{OP_SEARCH, _("Search")},
		{OP_CLEARFILTER, _("Clear")}
	};
	return hints;
}

void MessagesFormAction::finished_qna(Operation op)
{
	v.inside_qna(false);
	switch (op) {
	case OP_INT_START_SEARCH:
		searchphrase = qna_responses[0];
		apply_search = true;
		do_redraw = true;
		break;
	default:
		FormAction::finished_qna(op);
		break;
	}
}

std::string MessagesFormAction::title()
{
	return _("Messages");
}

std::string MessagesFormAction::make_colorstring(
	const std::vector<std::string>& colors)
{
	std::string result;
	if (colors.size() > 0) {
		if (colors[0] != "default") {
			result.append("fg=");
			result.append(colors[0]);
		}
		if (colors.size() > 1) {
			if (colors[1] != "default") {
				if (result.length() > 0) {
					result.append(",");
				}
				result.append("bg=");
				result.append(colors[1]);
			}
		}
		for (unsigned int i = 2; i < colors.size(); i++) {
			if (result.length() > 0) {
				result.append(",");
			}
			result.append("attr=");
			result.append(colors[i]);
		}
	}
	return result;
}

} // namespace newsboat
