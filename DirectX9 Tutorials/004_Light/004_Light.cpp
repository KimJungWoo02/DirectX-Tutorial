#include "DXUT.h"
#include "resource.h"


#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE)

ID3DXMesh* Teaport;

HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    D3DXCreateTeapot(pd3dDevice, &Teaport, 0);

    pd3dDevice->SetRenderState(D3DRS_LIGHTING, true);

    D3DMATERIAL9 mtrl;
    mtrl.Ambient = D3DXCOLOR(0,0, 0, 255);
    mtrl.Diffuse = D3DXCOLOR(0, 0, 0, 255);
    mtrl.Specular = D3DXCOLOR(0, 0, 0, 255);
    mtrl.Emissive = D3DXCOLOR(0, 0, 0, 255);
    mtrl.Power = 1.0f;

    pd3dDevice->SetMaterial(&mtrl);
    
    D3DLIGHT9 dir;
    ZeroMemory(&dir, sizeof(dir));
    dir.Type = D3DLIGHT_DIRECTIONAL;
    dir.Diffuse = D3DXCOLOR(255, 255, 255, 255);
    dir.Specular = D3DXCOLOR(255, 255, 255, 255) * 0.3f;
    dir.Ambient = D3DXCOLOR(255, 255, 255, 255) * 0.6f;
    dir.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);


    pd3dDevice->SetLight(0, &dir);
    pd3dDevice->LightEnable(0, true);

    pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
    pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

    //프로젝션과 뷰 행렬을 세팅하는 과정이다.
    //이 과정은 Camera 프로젝트를 참고할 것.
    D3DXVECTOR3 pos(0.0f, 1.0f, -3.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &pos, &target, &up);
    pd3dDevice->SetTransform(D3DTS_VIEW, &V);

    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (float)640 / (float)480, 1.0f, 1000.0f);
    pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj);


    return S_OK;
}

void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;
    V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 45, 50, 170 ), 1.0f, 0 ) );

    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        //이 내용은 추후 Camera 프로젝트에서 참고
        D3DXMATRIX yRot;
        static float y = 0.0f;

        D3DXMatrixRotationY(&yRot, y);
        y += fElapsedTime;

        if (y >= 6.28f)
            y = 0.0f;

        pd3dDevice->SetTransform(D3DTS_WORLD, &yRot);

        pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
        Teaport->DrawSubset(0);

        V( pd3dDevice->EndScene() );
    }
}

void CALLBACK OnD3D9DestroyDevice( void* pUserContext )
{
    Teaport->Release();
}


INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    DXUTSetCallbackD3D9FrameRender( OnD3D9FrameRender );
    DXUTSetCallbackD3D9DeviceDestroyed( OnD3D9DestroyDevice );

    DXUTInit( true, true );
    DXUTSetHotkeyHandling( true, true, true );
    DXUTSetCursorSettings( true, true );
    DXUTCreateWindow( L"Emptyproject" );
    DXUTCreateDevice( true, 640, 480 );

    DXUTMainLoop();

    return DXUTGetExitCode();
}


