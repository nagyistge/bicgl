#include  <internal_volume_io.h>
#include  <WS_graphics.h>

private  void  set_event_callbacks_for_current_window( BOOLEAN );

public  void  WS_initialize( void )
{
    static  BOOLEAN  initialized = FALSE;
    int              argc = 1;
    char             *argv[] = { "no_program_name" };

    if( !initialized )
    {
        initialized = TRUE;
        glutInit( &argc, argv );
    }
}

public  Status  WS_create_window(
    STRING                 title,
    int                    initial_x_pos,
    int                    initial_y_pos,
    int                    initial_x_size,
    int                    initial_y_size,
    BOOLEAN                colour_map_mode,
    BOOLEAN                double_buffer_flag,
    BOOLEAN                depth_buffer_flag,
    int                    n_overlay_planes,
    BOOLEAN                *actual_colour_map_mode,
    BOOLEAN                *actual_double_buffer_flag,
    BOOLEAN                *actual_depth_buffer_flag,
    int                    *actual_n_overlay_planes,
    WSwindow               window )
{
    unsigned  int      mode;
    int                screen_width, screen_height, used_size;

    if( initial_x_pos >= 0 && initial_y_pos >= 0 )
    {
        WS_get_screen_size( &screen_width, &screen_height );
        if( initial_y_size <= 0 )
            used_size = glutGet( (GLenum) GLUT_INIT_WINDOW_HEIGHT );
        else
            used_size = initial_y_size;

        glutInitWindowPosition( initial_x_pos, screen_height -
                                (initial_y_pos+used_size) );
    }

    if( initial_x_size > 0 && initial_y_size > 0 )
        glutInitWindowSize( initial_x_size, initial_y_size );

    mode = 0;

    if( colour_map_mode )
        mode |= GLUT_INDEX;
    else
        mode |= GLUT_RGB;

    if( double_buffer_flag )
        mode |= GLUT_DOUBLE;
    else
        mode |= GLUT_SINGLE;

    if( depth_buffer_flag )
        mode |= GLUT_DEPTH;

    mode |= GLUT_ALPHA;

    glutInitDisplayMode( mode );

    window->window_id = glutCreateWindow( title );

    if( window->window_id < 1 )
    {
        print_error( "Could not open GLUT window for OpenGL\n" );
        return( ERROR );
    }

    *actual_colour_map_mode = (glutGet((GLenum) GLUT_WINDOW_RGBA) != 1);
    *actual_double_buffer_flag = glutGet((GLenum) GLUT_WINDOW_DOUBLEBUFFER);
    *actual_depth_buffer_flag = (glutGet((GLenum) GLUT_WINDOW_DEPTH_SIZE) > 0);
    *actual_n_overlay_planes = 0;

    set_event_callbacks_for_current_window( *actual_n_overlay_planes );

    return( OK );
}

public  void  WS_delete_window(
    WSwindow  window )
{
    glutDestroyWindow( window->window_id );
}

public  Window_id  WS_get_current_window_id( void )
{
    return( glutGetWindow() );
}

public  BOOLEAN  WS_window_has_overlay_planes(
    WSwindow  window )
{
    return( FALSE );
}

private  void  set_window_normal_planes(
    WSwindow  window )
{
    glutSetWindow( window->window_id );
}

private  void  set_window_overlay_planes(
    WSwindow  window )
{
    glutSetWindow( window->window_id );
}

public  void  WS_set_current_window(
    WSwindow          window )
{
    WS_set_bitplanes( window, NORMAL_PLANES );
}


public  void  WS_set_bitplanes(
    WSwindow          window,
    Bitplane_types    bitplanes )
{
    if( bitplanes == OVERLAY_PLANES )
        set_window_overlay_planes( window );
    else
        set_window_normal_planes( window );
}


public  int    WS_get_n_overlay_planes( void )
{
    return( 0 );
}

public  Window_id   WS_get_window_id(
    WSwindow  window )
{
    return( window->window_id );
}

public  void  WS_get_window_position(
    WSwindow     window,
    int          *x_pos,
    int          *y_pos )
{
    Window_id  save_id;

    save_id = glutGetWindow();

    set_window_normal_planes( window );

    *x_pos = glutGet( (GLenum) GLUT_WINDOW_X );
    *y_pos = glutGet( (GLenum) GLUT_WINDOW_Y );

    glutSetWindow( save_id );
}

public  void  WS_get_window_size(
    WSwindow     window,
    int          *x_size,
    int          *y_size )
{
    Window_id  save_id;

    save_id = glutGetWindow();

    set_window_normal_planes( window );

    *x_size = glutGet( (GLenum) GLUT_WINDOW_WIDTH );
    *y_size = glutGet( (GLenum) GLUT_WINDOW_HEIGHT );

    glutSetWindow( save_id );
}

public  void  WS_set_colour_map_entry(
    WSwindow          window,
    Bitplane_types    bitplane,
    int               ind,
    Colour            colour )
{
    glutSetColor( ind, (float) get_Colour_r_0_1(colour),
                       (float) get_Colour_g_0_1(colour),
                       (float) get_Colour_b_0_1(colour) );
}

public  void  WS_set_overlay_colour_map_entry(
    WSwindow          window,
    int               ind,
    Colour            colour )
{
}

public  void  WS_swap_buffers(
    WSwindow          window )
{
    Window_id  save_id;

    save_id = glutGetWindow();

    set_window_normal_planes( window );

    glutSwapBuffers();

    glutSetWindow( save_id );
}

static  struct
        {
            int     height;
            void    *font;
        }        known_fonts[] =
              {
                {10, GLUT_BITMAP_HELVETICA_10},
                {12, GLUT_BITMAP_HELVETICA_12},
                {18, GLUT_BITMAP_HELVETICA_18},
                {10, GLUT_BITMAP_TIMES_ROMAN_10},
                {24, GLUT_BITMAP_TIMES_ROMAN_24},
                {8,  GLUT_BITMAP_8_BY_13},
                {9 , GLUT_BITMAP_9_BY_15}
              };


private  void  *lookup_font(
    Font_types       type,
    Real             size,
    int              *actual_height )
{
    Real    diff, min_diff;
    int     which, best;
    void    *font;

    if( type == FIXED_FONT )
    {
        font = GLUT_BITMAP_8_BY_13;
        if( actual_height != NULL )
            *actual_height = 13;
    }
    else
    {
        min_diff = 0.0;
        best = 0;
        for_less( which, 0, SIZEOF_STATIC_ARRAY(known_fonts) )
        {
            diff = FABS( (Real) known_fonts[which].height - size );
            if( which == 0 || diff < min_diff )
            {
                best = which;
                min_diff = diff;
            }
        }

        font = known_fonts[best].font;
        if( actual_height != NULL )
            *actual_height = known_fonts[best].height;
    }

    return( font );
}

public  void  WS_draw_text(
    Font_types  type,
    Real        size,
    STRING      string )
{
    int   i;
    void  *font;

    font = lookup_font( type, size, NULL );

    for_less( i, 0, string_length( string ) )
        glutBitmapCharacter( font, (int) string[i] );
}

public  Real  WS_get_character_height(
    Font_types       type,
    Real             size )
{
    int   height;

    (void) lookup_font( type, size, &height );

    return( (Real) height );
}

public  Real  WS_get_text_length(
    STRING           str,
    Font_types       type,
    Real             size )
{
    int    i, len;
    void   *font;

    font = lookup_font( type, size, NULL );

    len = 0;
    for_less( i, 0, (int) strlen( str ) )
        len += glutBitmapWidth( font, (int) str[i] );

    return( (Real) len );
}

public  void  WS_get_screen_size(
    int   *x_size, 
    int   *y_size  )
{
    *x_size = glutGet( (GLenum) GLUT_SCREEN_WIDTH );
    *y_size = glutGet( (GLenum) GLUT_SCREEN_HEIGHT );
}

static  void  (*display_callback) ( Window_id );
static  void  (*display_overlay_callback) ( Window_id );
static  void  (*resize_callback) ( Window_id, int, int, int, int );
static  void  (*key_down_callback) ( Window_id, int, int, int, int );
static  void  (*key_up_callback) ( Window_id, int, int, int, int );
static  void  (*mouse_motion_callback) ( Window_id, int, int );
static  void  (*left_down_callback) ( Window_id, int, int, int );
static  void  (*left_up_callback) ( Window_id, int, int, int );
static  void  (*middle_down_callback) ( Window_id, int, int, int );
static  void  (*middle_up_callback) ( Window_id, int, int, int );
static  void  (*right_down_callback) ( Window_id, int, int, int );
static  void  (*right_up_callback) ( Window_id, int, int, int );
static  void  (*iconify_callback) ( Window_id );
static  void  (*deiconify_callback) ( Window_id );
static  void  (*enter_callback) ( Window_id );
static  void  (*leave_callback) ( Window_id );
static  void  (*quit_callback) ( Window_id );

public  void  WS_set_update_function(
    void  (*func)( Window_id ) )
{
    display_callback = func;
}

public  void  WS_set_update_overlay_function(
    void  (*func)( Window_id ) )
{
    display_overlay_callback = func;
}

public  void  WS_set_resize_function(
    void  (*func)( Window_id, int, int, int, int ) )
{
    resize_callback = func;
}

public  void  WS_set_key_down_function(
    void  (*func)( Window_id, int, int, int, int ) )
{
    key_down_callback = func;
}

public  void  WS_set_key_up_function(
    void  (*func)( Window_id, int, int, int, int ) )
{
    key_up_callback = func;
}

public  void  WS_set_mouse_movement_function(
    void  (*func)( Window_id, int, int ) )
{
    mouse_motion_callback = func;
}

public  void  WS_set_left_mouse_down_function(
    void  (*func)( Window_id, int, int, int ) )
{
    left_down_callback = func;
}

public  void  WS_set_left_mouse_up_function(
    void  (*func)( Window_id, int, int, int ) )
{
    left_up_callback = func;
}

public  void  WS_set_middle_mouse_down_function(
    void  (*func)( Window_id, int, int, int ) )
{
    middle_down_callback = func;
}

public  void  WS_set_middle_mouse_up_function(
    void  (*func)( Window_id, int, int, int ) )
{
    middle_up_callback = func;
}

public  void  WS_set_right_mouse_down_function(
    void  (*func)( Window_id, int, int, int ) )
{
    right_down_callback = func;
}

public  void  WS_set_right_mouse_up_function(
    void  (*func)( Window_id, int, int, int ) )
{
    right_up_callback = func;
}

public  void  WS_set_iconify_function(
    void  (*func)( Window_id ) )
{
    iconify_callback = func;
}

public  void  WS_set_deiconify_function(
    void  (*func)( Window_id ) )
{
    deiconify_callback = func;
}

public  void  WS_set_enter_function(
    void  (*func)( Window_id ) )
{
    enter_callback = func;
}

public  void  WS_set_leave_function(
    void  (*func)( Window_id ) )
{
    leave_callback = func;
}

public  void  WS_set_quit_function(
    void  (*func)( Window_id ) )
{
    quit_callback = func;
}

private  int  get_keyboard_modifiers( void )
{
    int  modifier, glut_mod;

    modifier = 0;
    glut_mod = glutGetModifiers();

    if( (glut_mod & GLUT_ACTIVE_SHIFT) != 0 )
        modifier |= SHIFT_KEY_BIT;
    if( (glut_mod & GLUT_ACTIVE_CTRL) != 0 )
        modifier |= CTRL_KEY_BIT;
    if( (glut_mod & GLUT_ACTIVE_ALT) != 0 )
        modifier |= ALT_KEY_BIT;

    return( modifier );
}

private  int  flip_mouse_y(
    int   mouse_y )
{
    return( glutGet( (GLenum) GLUT_WINDOW_HEIGHT ) - 1 - mouse_y );
}

private  void  display_function( void )
{
    (*display_callback) ( WS_get_current_window_id() );
}

private  void  display_overlay_function( void )
{
    (*display_overlay_callback) ( WS_get_current_window_id() );
}

private  void  resize_function(
    int   width,
    int   height )
{
    int   x, y;

    x = glutGet( (GLenum) GLUT_WINDOW_X );
    y = glutGet( (GLenum) GLUT_WINDOW_Y );

    (*resize_callback) ( WS_get_current_window_id(), x, y, width, height );
}

private  void  keyboard_function(
    unsigned  char  key,
    int             x,
    int             y )
{
    y = flip_mouse_y( y );
    (*key_down_callback) ( WS_get_current_window_id(), (int) key, x, y,
                           get_keyboard_modifiers() );
}

private  void  special_keyboard_function(
    int     key,
    int     x,
    int     y )
{
    int   translated;

    y = flip_mouse_y( y );

    translated = -1000;

    switch( key )
    {
    case GLUT_KEY_LEFT:     translated = LEFT_ARROW_KEY;  break;
    case GLUT_KEY_RIGHT:    translated = RIGHT_ARROW_KEY;  break;
    case GLUT_KEY_UP:       translated = UP_ARROW_KEY;  break;
    case GLUT_KEY_DOWN:     translated = DOWN_ARROW_KEY;  break;
    }

    if( translated != -1000 )
        (*key_down_callback) ( WS_get_current_window_id(), translated,
                               x, y, get_keyboard_modifiers() );
}

private  void  mouse_button_function(
    int     button,
    int     state,
    int     x,
    int     y )
{
    int         modifiers;
    Window_id   window_id;

    window_id = WS_get_current_window_id();
    modifiers = get_keyboard_modifiers();
    y = flip_mouse_y( y );

    switch( button )
    {
    case GLUT_LEFT_BUTTON:
        if( state == GLUT_DOWN )
            (*left_down_callback) ( window_id, x, y, modifiers );
        else
            (*left_up_callback) ( window_id, x, y, modifiers );
        break;

    case GLUT_MIDDLE_BUTTON:
        if( state == GLUT_DOWN )
            (*middle_down_callback) ( window_id, x, y, modifiers );
        else
            (*middle_up_callback) ( window_id, x, y, modifiers );
        break;

    case GLUT_RIGHT_BUTTON:
        if( state == GLUT_DOWN )
            (*right_down_callback) ( window_id, x, y, modifiers );
        else
            (*right_up_callback) ( window_id, x, y, modifiers );
        break;
    }
}

private  void  mouse_motion_function(
    int     x,
    int     y )
{
    y = flip_mouse_y(y);
    (*mouse_motion_callback) ( WS_get_current_window_id(), x, y );
}

private  void  entry_function(
    int     state )
{
    Window_id   window_id;

    window_id = WS_get_current_window_id();

    if( state == GLUT_LEFT )
        (*leave_callback) ( window_id );
    else
        (*enter_callback) ( window_id );
}

private  void  set_event_callbacks_for_current_window(
    int   n_overlay_planes )
{
    glutDisplayFunc( display_function );

    if( n_overlay_planes > 0 )
        glutOverlayDisplayFunc( display_overlay_function );

    glutReshapeFunc( resize_function );
    glutKeyboardFunc( keyboard_function );
    glutSpecialFunc( special_keyboard_function );
    glutMouseFunc( mouse_button_function );
    glutMotionFunc( mouse_motion_function );
    glutPassiveMotionFunc( mouse_motion_function );
    glutEntryFunc( entry_function );
}

typedef struct
{
    void  (*function) ( void * );
    void              *data;
    BOOLEAN           active;
} callback_info_struct;

static  callback_info_struct   *timers;
static  int                    n_timers = 0;

private  void  global_timer_function(
    int   index )
{
    if( index < 0 || index >= n_timers )
    {
        handle_internal_error( "global_timer_function" );
        return;
    }

    (*timers[index].function)( timers[index].data );

    timers[index].active = FALSE;
}

public  void  WS_add_timer_function(
    Real          seconds,
    void          (*func) ( void * ),
    void          *data )
{
    int                   i;
    callback_info_struct  info;

    for_less( i, 0, n_timers )
    {
        if( !timers[i].active )
            break;
    }

    if( i >= n_timers )
    {
        info.function = func;
        info.data = data;

        ADD_ELEMENT_TO_ARRAY( timers, n_timers, info, 1 );
    }

    timers[i].active = TRUE;

    glutTimerFunc( (unsigned int) (1000.0 * seconds + 0.5),
                   global_timer_function, i );
}

static  callback_info_struct   *idles;
static  int                    n_idles = 0;

private  void  global_idle_function( void )
{
    int   i;

    for_less( i, 0, n_idles )
    {
        (*idles[i].function) ( idles[i].data );
    }
}

public  void  WS_add_idle_function(
    void  (*func) ( void * ),
    void          *data )
{
    callback_info_struct  info;

    if( n_idles == 0 )
        glutIdleFunc( global_idle_function );

    info.function = func;
    info.data = data;

    ADD_ELEMENT_TO_ARRAY( idles, n_idles, info, 1 );
}

public  void  WS_remove_idle_function(
    void  (*func) ( void * ),
    void          *data )
{
    int   i;

    for_less( i, 0, n_idles )
    {
        if( idles[i].function == func && idles[i].data == data )
            break;
    }

    if( i >= n_idles )
    {
        handle_internal_error( "WS_delete_idle_func" );
        return;
    }

    DELETE_ELEMENT_FROM_ARRAY( idles, n_idles, i, 1 );

    if( n_idles == 0 )
        glutIdleFunc( NULL );
}

public  void  WS_event_loop( void )
{
    glutMainLoop();
}

public  void  WS_set_update_flag(
    WSwindow   window  )
{
    Window_id  save_id;

    save_id = glutGetWindow();

    set_window_normal_planes( window );

    glutPostRedisplay();

    glutSetWindow( save_id );
}

