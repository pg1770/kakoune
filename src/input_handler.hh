#ifndef input_handler_hh_INCLUDED
#define input_handler_hh_INCLUDED

#include "color.hh"
#include "completion.hh"
#include "context.hh"
#include "editor.hh"
#include "keys.hh"
#include "string.hh"
#include "utils.hh"

namespace Kakoune
{

class Editor;

enum class MenuEvent
{
    Select,
    Abort,
    Validate
};
using MenuCallback = std::function<void (int, MenuEvent, Context&)>;

enum class PromptEvent
{
    Change,
    Abort,
    Validate
};
using PromptCallback = std::function<void (const String&, PromptEvent, Context&)>;
using KeyCallback = std::function<void (Key, Context&)>;

class InputMode;
enum class InsertMode : unsigned;

class InputHandler : public SafeCountable
{
public:
    InputHandler(Editor& editor, String name = "");
    ~InputHandler();

    // switch to insert mode
    void insert(InsertMode mode);
    // repeat last insert mode key sequence
    void repeat_last_insert();

    // enter prompt mode, callback is called on each change,
    // abort or validation with corresponding PromptEvent value
    // returns to normal mode after validation if callback does
    // not change the mode itself
    void prompt(const String& prompt, ColorPair prompt_colors,
                Completer completer, PromptCallback callback);
    void set_prompt_colors(ColorPair prompt_colors);

    // enter menu mode, callback is called on each selection change,
    // abort or validation with corresponding MenuEvent value
    // returns to normal mode after validation if callback does
    // not change the mode itself
    void menu(memoryview<String> choices,
              MenuCallback callback);

    // execute callback on next keypress and returns to normal mode
    // if callback does not change the mode itself
    void on_next_key(KeyCallback callback);

    // process the given key
    void handle_key(Key key);

    void start_recording(char reg);
    bool is_recording() const;
    void stop_recording();
    char recording_reg() const { return m_recording_reg; }

    void reset_normal_mode();

    Context& context() { return m_context; }
    const Context& context() const { return m_context; }

    String mode_string() const;
    void clear_mode_trash();
private:
    Context m_context;

    friend class InputMode;
    std::unique_ptr<InputMode> m_mode;
    std::vector<std::unique_ptr<InputMode>> m_mode_trash;

    void change_input_mode(InputMode* new_mode);

    using Insertion = std::pair<InsertMode, std::vector<Key>>;
    Insertion m_last_insert = {InsertMode::Insert, {}};

    char   m_recording_reg = 0;
    String m_recorded_keys;
};

}

#endif // input_handler_hh_INCLUDED
