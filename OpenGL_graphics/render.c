 
#include  <internal_volume_io.h>
#include  <graphics.h>

public  void  GS_set_shade_model(
    Shading_types  type )
{
#ifndef  TWO_D_ONLY
    if( type == GOURAUD_SHADING )
        glShadeModel( GL_SMOOTH );
    else
        glShadeModel( GL_FLAT );
#endif
}

public  void  GS_turn_off_blend_function()
{
    glDisable( GL_BLEND );
    glBlendFunc( GL_ONE, GL_ZERO );
}

public  void  GS_turn_on_blend_function()
{
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_BLEND );
}

public  void  GS_set_lighting_state(
    Gwindow      window,
    BOOLEAN      state )   /* ARGSUSED */
{
#ifndef  TWO_D_ONLY
    if( state )
        glEnable( GL_LIGHTING );
    else
        glDisable( GL_LIGHTING );
#endif
}

public  void  GS_backface_culling_state(
    BOOLEAN     state )
{
#ifndef  TWO_D_ONLY
    if( state )
    {
        glEnable( GL_CULL_FACE );
        glCullFace( GL_BACK );
    }
    else
        glDisable( GL_CULL_FACE );
#endif
}

public  void  GS_set_n_curve_segments(
    int      n_segments )
{
#ifndef  TWO_D_ONLY
#ifdef  TO_DO
    curveprecision( n_segments );
#endif
#endif
}