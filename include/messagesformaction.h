#ifndef NEWSBOAT_MESSAGESFORMACTION_H_
#define NEWSBOAT_MESSAGESFORMACTION_H_

#include "formaction.h"
#include "listwidget.h"

namespace newsboat {

class MessagesFormAction : public FormAction {
public:
	MessagesFormAction(View&, std::string formstr, ConfigContainer* cfg);
	~MessagesFormAction() override = default;
	void prepare() override;
	void init() override;
	const std::vector<KeyMapHintEntry>& get_keymap_hint() const override;
	std::string id() const override
	{
		return "messages";
	}
	std::string title() override;

	void finished_qna(Operation op) override;

protected:
	std::string main_widget() const override
	{
		return "messages";
	}

private:
	bool process_operation(Operation op,
		const std::vector<std::string>& args,
		BindingType bindingType = BindingType::BindKey) override;
	std::string make_colorstring(const std::vector<std::string>& colors);
	bool quit;
	bool apply_search;
	std::string searchphrase;
	ListWidget listview;
};

} // namespace newsboat

#endif /* NEWSBOAT_MESSAGESFORMACTION_H_ */
