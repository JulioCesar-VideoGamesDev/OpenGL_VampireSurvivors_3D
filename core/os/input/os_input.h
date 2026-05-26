#pragma once

struct Input_Desc
{
};

enum class Cursor_Mode
{
      Default
    , Confined
    , Hidden
};

namespace Key_State
{
    // @Note: Common use case.
    // 
    // Pressed  -> Start | Down
    // Repeat   -> Down
    // Released -> End

    enum Type : u32 
    {
          None  = 1 << 0
        , Down  = 1 << 1
        , Start = 1 << 2 
        , End   = 1 << 3
    };
}

// @Note: We match placements in ASCII table.
namespace Key_Code
{
    enum Type : u32 
    {
          Unknown   = 0
        
        // *** ASCII (REAL CODES) ***

        , Backspace    = 8
        , Tab          = 9
        , Linefeed     = 10
        , Enter        = 13
        , Escape       = 27
        , Space        = 32

        // @Note: Here live ASCII letters, digits and symbols via direct char input.
        // They are not enumerated because they are received as char or UTF-32 text.

        // *** END OF ASCII ***
        // Begin custom keycodes (physical keys)

        , _BEGIN_NON_ASCII = 512 // @Note: Explicit boundary to avoid collisions.

        , Delete       
        , Arrow_Up     
        , Arrow_Down   
        , Arrow_Left   
        , Arrow_Right  
        , Page_Up      
        , Page_Down    
        , Home         
        , End          
        , Insert       
        , Pause        
        , Scroll_Lock  
        , Alt          
        , Ctrl         
        , Shift        
        , CMD          

        // *** FUNCTION KEYS ***
        , F1  , F2  , F3  , F4  , F5  , F6  , F7  , F8  , F9 , F10
        , F11 , F12 , F13 , F14 , F15 , F16 , F17 , F18 , F19
        , F20 , F21 , F22 , F23 , F24

        , Print_Screen

        // *** MOUSE AS BUTTON INPUT KEYS ***
        , Mouse_Left
        , Mouse_Middle
        , Mouse_Right

        // *** End ***
        , Count
    };
};

struct Input_Event
{
    enum Type : u32
    {
          None
        , Key
        , Window
        , Mouse_Move
        , Mouse_Wheel
        , Quit
    };
    
    Type type = Type::None;

    u32 key_code  = Key_Code::Unknown;
    u32 key_state = Key_State::None;
    s32 wheel_delta = 0; //@Note +up -down

    s32 window_x = 0;
    s32 window_y = 0;
    
    s32 mouse_x = 0;
    s32 mouse_y = 0;

    s32 mouse_delta_x = 0;
    s32 mouse_delta_y = 0;
};

fn os_input_init(const Input_Desc& ds) -> bool;
fn os_input_done() -> void;
fn os_poll_events() -> void;
fn os_events_this_frame() -> const Array<Input_Event>&;
fn os_key_down(u32 key_code) -> bool;
fn os_set_cursor_mode(Cursor_Mode mode) -> void;

class IInput
{
public:
    using Ptr = IInput*;
    static auto create(const Input_Desc& ds = {})->Ptr;

    virtual ~IInput() = default;

    virtual auto poll_events() const -> void = 0;
    virtual auto events_this_frame() const -> const Array<Input_Event>& = 0;
    virtual auto key_down(u32 key_code) const -> bool = 0;
    virtual auto set_cursor_mode(Cursor_Mode mode) -> void = 0;
};
