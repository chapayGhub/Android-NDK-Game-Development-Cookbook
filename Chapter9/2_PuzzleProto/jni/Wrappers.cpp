#include <stdlib.h>
#include <jni.h>
#include <android/log.h>
#include "Multitouch.h"
#include <LGLAPI.h>
#include <LGL.h>

#include "Wrapper_Callbacks.h"

#include <string>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Game1", __VA_ARGS__))

std::string g_ExternalStorage;
std::string g_APKName;

int g_Width = 1;
int g_Height = 1;

extern sLGLAPI* LGL3;

std::string ConvertJString( JNIEnv* env, jstring str )
{
	if ( !str ) { std::string(); }

	const jsize len = env->GetStringUTFLength( str );
	const char* strChars = env->GetStringUTFChars( str, ( jboolean* )0 );

	std::string Result( strChars, len );

	env->ReleaseStringUTFChars( str, strChars );

	return Result;
}

struct sSendMotionData
{
	int ContactID;
	eMotionFlag Flag;
	LVector2 Pos;
	bool Pressed;
};

clMutex g_MotionEventsQueueMutex;
std::vector<sSendMotionData> g_MotionEventsQueue;

void GestureHandler_SendMotion( int ContactID, eMotionFlag Flag, LVector2 Pos, bool Pressed );

extern "C"
{
	JNIEXPORT void JNICALL Java_com_packtpub_ndkcookbook_game1_Game1Activity_OnCreateNative( JNIEnv* env, jobject obj, jstring ExternalStorage )
	{
		g_ExternalStorage = ConvertJString( env, ExternalStorage );
	}

	JNIEXPORT void JNICALL Java_com_packtpub_ndkcookbook_game1_Game1Activity_SetAPKName( JNIEnv* env, jobject obj, jstring APKName )
	{
		g_APKName = ConvertJString( env, APKName );

		LOGI( "APKName = %s", g_APKName.c_str() );
	}

	JNIEXPORT void JNICALL Java_com_packtpub_ndkcookbook_game1_Game1Activity_SendMotion( JNIEnv* env, jobject obj, int PointerID, int x, int y, bool Pressed, int Flag )
	{
		sSendMotionData M;
		M.ContactID = PointerID;
		M.Flag = ( eMotionFlag )Flag;
		M.Pos = LVector2( ( float )x / ( float )g_Width, ( float )y / ( float )g_Height );
		M.Pressed = Pressed;

		LMutex Lock( &g_MotionEventsQueueMutex );
		g_MotionEventsQueue.push_back( M );
	}

	JNIEXPORT void JNICALL Java_com_packtpub_ndkcookbook_game1_Game1Activity_SetSurface( JNIEnv* env, jclass clazz, jobject javaSurface )
	{
		if ( LGL3 ) { delete( LGL3 ); }

		LGL3 = new sLGLAPI;

		LGL::clGLExtRetriever* OpenGL;
		OpenGL = new LGL::clGLExtRetriever;
		OpenGL->Reload( LGL3 );
		delete( OpenGL );

		OnStart( g_APKName );
	}

	JNIEXPORT void JNICALL Java_com_packtpub_ndkcookbook_game1_Game1Activity_SetSurfaceSize( JNIEnv* env, jclass clazz, int Width, int Height )
	{
		LOGI( "SurfaceSize: %i x %i", Width, Height );

		g_Width  = Width;
		g_Height = Height;
	}

	JNIEXPORT void JNICALL Java_com_packtpub_ndkcookbook_game1_Game1Activity_DrawFrame( JNIEnv* env, jobject obj )
	{
		// send events synchronously
		{
			LMutex Lock( &g_MotionEventsQueueMutex );

			for ( auto m : g_MotionEventsQueue )
			{
				GestureHandler_SendMotion( m.ContactID, m.Flag, m.Pos, m.Pressed );
			}

			g_MotionEventsQueue.clear();
		}

		GenerateTicks();
	}

	JNIEXPORT void JNICALL Java_com_packtpub_ndkcookbook_game1_Game1Activity_ExitNative( JNIEnv* env, jobject obj )
	{
		OnStop();

		exit( 0 );
	}

} // extern "C"

void ExitApp()
{
	OnStop();

	exit( 0 );
}
