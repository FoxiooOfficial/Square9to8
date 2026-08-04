
/***********************************************************/
//
//  Copyright (c) 2024-2026 Foxioo 
//  Project repository page: https://github.com/FoxiooOfficial/Square9to8
//
/***********************************************************/

#define STRICT
#include <windows.h>
#include <tchar.h>
#include <D3DX8.h>
#include "D3DApp.h"

#include <cmath>

#pragma comment(lib, "legacy_stdio_definitions.lib")
#pragma comment(lib, "d3d8.lib")
#pragma comment(lib, "d3dx8.lib")
#pragma comment(lib, "winmm.lib")

/***********************************************************/

#define D3DFVF_VERTEX_CUSTOM_STATIC (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
struct SETFVF_VERTEX_CUSTOM_STATIC
{
    FLOAT x, y, z, rhw;
    DWORD color;
    FLOAT u, v;
};

#define D3DFVF_VERTEX_CUSTOM (D3DFVF_XYZ | D3DFVF_DIFFUSE)
struct SETFVF_VERTEX_CUSTOM {
    FLOAT x, y, z;
    DWORD color;
};

/***********************************************************/
/*  
    Colors!!11!!1! 
    support function that convert the format like with shaders—where a value of 0.0 to 1.0 represents a color value of 0 to 255;
*/
static D3DCOLOR cfloat4(float r, float g, float b, float a)
{
    int ri = (int)(r * 255.0f);
    int gi = (int)(g * 255.0f);
    int bi = (int)(b * 255.0f);
    int ai = (int)(a * 255.0f);

    return D3DCOLOR_RGBA(ri, gi, bi, ai);
}

static D3DCOLOR cfloat3(float r, float g, float b)
{
    int ri = (int)(r * 255.0f);
    int gi = (int)(g * 255.0f);
    int bi = (int)(b * 255.0f);

    return D3DCOLOR_XRGB(ri, gi, bi);
}

#define COLOR_BLACK     cfloat3(0.0f, 0.0f, 0.0f)
#define COLOR_RED       cfloat3(1.0f, 0.0f, 0.0f)
#define COLOR_GREEN     cfloat3(0.0f, 1.0f, 0.0f)
#define COLOR_BLUE      cfloat3(0.0f, 0.0f, 1.0f)
#define COLOR_WHITE     cfloat3(1.0f, 1.0f, 1.0f)

/***********************************************************/
/*
    Scene Settings
*/

#define COLOR_BACKGROUND_RGB    COLOR_BLUE
#define COLOR_BACKGROUND_A      1.0f

#define D3D_ERASE_RENDER        1
#define D3D_ENABLE_ALPHA        1
#define D3D_ENABLE_TEXTURE      1
#define D3D_ENABLE_TEXTURE_BG   1

#define VERTEX_POINTS_COUNT     4
#define VERTEX_TRANGLE_COUNT    2

#define OBJ_TEXTURE_PATH        _T("res/HungryFish.png")
#define BG_TEXTURE_PATH         _T("res/Background.png")

/***********************************************************/
/*
    Application class;
    application class

    bottom text :sob:
*/
class CMyD3DApplication : public CD3DApplication
{
    /* Object */
        LPDIRECT3DVERTEXBUFFER8     D3D_OBJ_VERTEX_BUFFOR   = NULL;
        LPDIRECT3DTEXTURE8          D3D_OBJ_TEXTURE         = NULL;

    /* Background */
        LPDIRECT3DVERTEXBUFFER8     D3D_BG_VERTEX_BUFFOR    = NULL;
        LPDIRECT3DTEXTURE8          D3D_BG_TEXTURE          = NULL;

    unsigned long long TIMER = 0;

    protected:
        // Initialize scene objects
        HRESULT RestoreDeviceObjects() override
        {
            float screenW = (float)m_d3dsdBackBuffer.Width;
            float screenH = (float)m_d3dsdBackBuffer.Height;

            float centerX = screenW / 2.0f;
            float centerY = screenH / 2.0f;

            /* Background */
            {
                #if D3D_ENABLE_TEXTURE_BG
                    if (SUCCEEDED(D3DXCreateTextureFromFile(m_pd3dDevice, BG_TEXTURE_PATH, &D3D_BG_TEXTURE)))
                    {
                        D3DSURFACE_DESC bg;
                        D3D_BG_TEXTURE->GetLevelDesc(0, &bg);

                            float bgscale = 2.0f;

                                float bgRX = screenW / (float)bg.Width / bgscale;
                                float bgRY = screenH / (float)bg.Height / bgscale;

                                SETFVF_VERTEX_CUSTOM_STATIC verticesbg[] =
                                {
                                    { 0.0f,     0.0f,       0.0f,   1.0f,   COLOR_WHITE,    0.0f,   0.0f },
                                    { screenW,  0.0f,       0.0f,   1.0f,   COLOR_WHITE,    bgRX,   0.0f },
                                    { 0.0f,     screenH,    0.0f,   1.0f,   COLOR_WHITE,    0.0f,   bgRY },
                                    { screenW,  screenH,    0.0f,   1.0f,   COLOR_WHITE,    bgRX,   bgRY }
                                };

                        if (SUCCEEDED(m_pd3dDevice->CreateVertexBuffer(sizeof(verticesbg), D3DUSAGE_WRITEONLY, D3DFVF_VERTEX_CUSTOM_STATIC, D3DPOOL_MANAGED, &D3D_BG_VERTEX_BUFFOR)))
                        {
                            VOID* verticesbg_p;

                            if (SUCCEEDED(D3D_BG_VERTEX_BUFFOR->Lock(0, sizeof(verticesbg), (BYTE**)&verticesbg_p, 0)))
                            {
                                memcpy(verticesbg_p, verticesbg, sizeof(verticesbg));
                                D3D_BG_VERTEX_BUFFOR->Unlock();
                            }
                        }
                        else
                            OutputDebugString(_T("Error! Cannot load texture for background;\n"));
                    }
            #endif
            }

            /* Object */
            {
                if (FAILED(m_pd3dDevice->CreateVertexBuffer(VERTEX_POINTS_COUNT * sizeof(SETFVF_VERTEX_CUSTOM_STATIC), 
                                                            D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                                                            D3DFVF_VERTEX_CUSTOM_STATIC,
                                                            D3DPOOL_DEFAULT,
                                                            &D3D_OBJ_VERTEX_BUFFOR)))
                {
                    return E_FAIL;
                }
             
                    // Settings trangle
                    float scale = 8.0f;
                    float sizeX = 36.0f * scale;
                    float sizeY = 33.0f * scale;

                        float posX = centerX - sizeX / 2.0f;
                        float posY = centerY - sizeY / 2.0f;
                        float posZ = 0.0f;

                        SETFVF_VERTEX_CUSTOM_STATIC vertices[] =
                        {
                            { posX,         posY,         posZ,     1.0f,   COLOR_WHITE,    0.0f,   0.0f },
                            { posX + sizeX, posY,         posZ,     1.0f,   COLOR_WHITE,    1.0f,   0.0f },
                            { posX,         posY + sizeY, posZ,     1.0f,   COLOR_WHITE,    0.0f,   1.0f },
                            { posX + sizeX, posY + sizeY, posZ,     1.0f,   COLOR_WHITE,    1.0f,   1.0f }
                        };

                    VOID* vertices_p;

                    if (FAILED(D3D_OBJ_VERTEX_BUFFOR->Lock(0, sizeof(vertices), (BYTE**)&vertices_p, 0)))
                        return E_FAIL;

                memcpy(vertices_p, vertices, sizeof(vertices));
                D3D_OBJ_VERTEX_BUFFOR->Unlock();

                // Set texture
                #if D3D_ENABLE_TEXTURE
                    HRESULT _OBJ_TEXTURE_LOAD = D3DXCreateTextureFromFile(m_pd3dDevice, OBJ_TEXTURE_PATH, &D3D_OBJ_TEXTURE);
                    if (FAILED(_OBJ_TEXTURE_LOAD))
                        OutputDebugString(_T("Error! Cannot load texture;\n"));
                #else
                    OutputDebugString(_T("Warning! Ignoring drawing texture\n"));
                #endif
            }

            return S_OK;
        }

        // InvalidateDeviceObjects()
        // read the name of the function; bruh
        HRESULT InvalidateDeviceObjects() override
        {
            if (D3D_OBJ_VERTEX_BUFFOR)
            { 
                D3D_OBJ_VERTEX_BUFFOR->Release(); 
                D3D_OBJ_VERTEX_BUFFOR = NULL;
            }
            if (D3D_OBJ_TEXTURE)
            { 
                D3D_OBJ_TEXTURE->Release();
                D3D_OBJ_TEXTURE = NULL;
            }

            if (D3D_BG_VERTEX_BUFFOR)
            { 
                D3D_BG_VERTEX_BUFFOR->Release(); 
                D3D_BG_VERTEX_BUFFOR = NULL;
            }
            if (D3D_BG_TEXTURE)
            { 
                D3D_BG_TEXTURE->Release();
                D3D_BG_TEXTURE = NULL; 
            }

            return S_OK;
        }

        // Rendering scene
        HRESULT Render() override
        {
            #if D3D_ERASE_RENDER
                m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, COLOR_BACKGROUND_RGB, COLOR_BACKGROUND_A, 0);
            #endif

            if (SUCCEEDED(m_pd3dDevice->BeginScene()))
            {
                m_pd3dDevice->SetVertexShader(D3DFVF_VERTEX_CUSTOM_STATIC);

                    /* Background */
                    #if D3D_ENABLE_TEXTURE_BG
                        m_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP); // Loop in X
                        m_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP); // Loop in Y

                        m_pd3dDevice->SetStreamSource(0, D3D_BG_VERTEX_BUFFOR, sizeof(SETFVF_VERTEX_CUSTOM_STATIC));
                        m_pd3dDevice->SetTexture(0, D3D_BG_TEXTURE);
                        m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, VERTEX_TRANGLE_COUNT);
                    #else
                        m_pd3dDevice->SetTexture(0, D3D_BG_TEXTURE);
                    #endif

                       /* Object */
                        #if D3D_ENABLE_ALPHA
                            m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
                            m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
                            m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
                        #endif

                        #if D3D_ENABLE_TEXTURE
                            m_pd3dDevice->SetTexture(0, D3D_OBJ_TEXTURE);
                        #else
                            m_pd3dDevice->SetTexture(0, NULL);
                        #endif

                m_pd3dDevice->SetStreamSource(0, D3D_OBJ_VERTEX_BUFFOR, sizeof(SETFVF_VERTEX_CUSTOM_STATIC));
                m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, VERTEX_TRANGLE_COUNT);

                m_pd3dDevice->EndScene();
            }

            TIMER++;

            return S_OK;
        }

    public:
        // Windows settings;
        CMyD3DApplication()
        {
            m_strWindowTitle = (TCHAR*)_T("Hello World!");
            m_bUseDepthBuffer = FALSE;
        }
};

/***********************************************************/
/*
    Main function;
    Calls the creation of a window from the CMyD3DApplication class;
*/
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
    CMyD3DApplication d3d8app;

    if (FAILED(d3d8app.Create(hInst)))
        return 0;

    return d3d8app.Run();
}