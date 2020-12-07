#include "DXUT.h"
#include "resource.h"

/*
    인덱스 버퍼 프로젝트

     인덱스 버퍼와 정점 버퍼와의 차이는 정점 버퍼는 직접 버텍스를 입력하여 값을 저장하지만 인덱스 버퍼는
     미리 정의된 정점들의 인덱스를 지정하여 저장하는 방식이다. 인덱스 버퍼보다 버텍스 버퍼가 좀 더 자원을 아낄 수 있다는 장점이 있다.

     (참고)정점 버퍼와의 차이가 많이 없어 겹치는 부분은 주석을 제외하였다.
*/

struct CUSTOMVERTEX
{
    float x, y, z;
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ)

CUSTOMVERTEX vertices[] =
{
    {-1.0f, -1.0f, -1.0f},
    {-1.0f,  1.0f, -1.0f},
    { 1.0f,  1.0f, -1.0f},
    { 1.0f, -1.0f, -1.0f},
    {-1.0f, -1.0f,  1.0f},
    {-1.0f,  1.0f,  1.0f},
    { 1.0f,  1.0f,  1.0f},
    { 1.0f, -1.0f,  1.0f},
};

//위에 정의된 vertices에 해당하는 인덱스 정보를 담아두는 배열
WORD Indices[] =
{
    0,1,2,0,2,3,
    4,6,5,4,7,6,
    4,5,1,4,1,0,
    3,2,6,3,6,7,
    1,5,6,1,6,2,
    4,0,3,4,3,7
};

LPDIRECT3DINDEXBUFFER9 g_pIB;
LPDIRECT3DVERTEXBUFFER9 g_pVB;

HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    /*

    디바이스에게 인덱스 버퍼를 생성(할당)하는 함수이다.
    첫번째, 두번째와 뒤에서 1,2,3번쨰 인자는 정점 버퍼와 같다.
    나머지 인자값인 Format은 인덱스의 데이터 타입의 포맷이다.
    현재는 WORD형이므로 16바이트의 사이즈를 가르키는 D3DFMT_INDEX16를 넘겨줬다.

    */
    pd3dDevice->CreateIndexBuffer(36 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &g_pIB, nullptr);
    pd3dDevice->CreateVertexBuffer(8 * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &g_pVB, nullptr);


    CUSTOMVERTEX* vertice = nullptr;
    g_pVB->Lock(0, sizeof(vertices), (void**)&vertice, 0);
    memcpy(vertice, vertices, sizeof(vertices));
    g_pVB->Unlock();

    WORD* word = nullptr;
    g_pIB->Lock(0, sizeof(Indices), (void**)&word, 0);
    memcpy(word, Indices, sizeof(Indices));
    g_pIB->Unlock();


    //프로젝션과 뷰 행렬을 세팅하는 과정이다.
    //이 과정은 Camera 프로젝트를 참고할 것.
    D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    D3DXMatrixLookAtLH(&V, &position, &target, &up);

    pd3dDevice->SetTransform(D3DTS_VIEW, &V);

    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(
        &proj,
        D3DX_PI * 0.5f, // 90 - degree
        (float)640 / (float)480,
        1.0f,
        1000.0f);
    pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj);

    //렌더 상태를 결정하는 함수
    //첫번째 인자값은 현재 렌더 스테이트를 설정하는 열거형(enum)
    //두번째 인자값은 첫번째 인자값에 따라 렌더 상태를 결정하는 열거형(enum)
    //현재 이 상태는 도형의 선만 출력하는 형태(면을 제거한 상태)
    pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    return S_OK;
}

void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;
    V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 45, 50, 170 ), 1.0f, 0 ) );

    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        //오브젝트를 회전하기 위해 World 좌표를 변환하는 것.
        //이것에 대한 자세한 내용은  Matrix 프로젝트를 참고할 것.
        D3DXMATRIX Rx, Ry;
        D3DXMatrixRotationX(&Rx, 3.14f / 4.0f);
        static float y = 0.0f;
        D3DXMatrixRotationY(&Ry, y);
        y += fElapsedTime;

        if (y >= 6.28f)
            y = 0.0f;

        D3DXMATRIX p = Rx * Ry;
        pd3dDevice->SetTransform(D3DTS_WORLD, &p);

        
        pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
        //인덱스 버퍼를 지정해준다.
        pd3dDevice->SetIndices(g_pIB);
        pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
        //이 함수는 인덱스 버퍼에 담긴 값을 실질적으로 그려주는 함수이다.(참조 : https://themangs.tistory.com/entry/d3d%EC%9D%98-DrawIndexedPrimitive-%ED%95%A8%EC%88%98%EC%97%90-%EB%8C%80%ED%95%B4%EC%84%9C)
        /*
            인자값
            Type - 그리고자하는 기본형 타입(Vertex Buffer와 동일)
            BaseVertexIndex - 이번 호출에 이용될 인덱스에 더해질 기반 번호를 지정한다.
            MinIndex - 참조할 최소 인덱스 값
            Numvertex - 이번 호출에 참조될 버텍스 수
            StartIndex - 인덱스 버퍼 내에서 읽기를 시작할 요소로의 인덱스
            PrimitiveCount - 그리고자 하는 기본형의 수(폴리곤의 수)
        
        */
        pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 0, 0, 12);

        V( pd3dDevice->EndScene() );
    }
}

void CALLBACK OnD3D9DestroyDevice( void* pUserContext )
{
    g_pIB->Release();
    g_pVB->Release();
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


