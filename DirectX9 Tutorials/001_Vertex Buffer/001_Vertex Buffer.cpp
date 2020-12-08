#include "DXUT.h"
#include "resource.h"


//Space 키를 누르면 쉐이딩 모드를 전환할 수 있다.

/*

참고 링크
https://hellowoori.tistory.com/31

CUSTOMVERTEX라는 정점 정보를 담은 구조체를 선언하였다. 이 구조체는 현재 정점의 위치 값과 rhw값 그리고 정점의 색깔 값이 담겨있다.
DirectX에서는 이렇게 개발자가 커스텀된 정점 구조체를 활용하여 정점 포맷을 구성하고 있다.
정점 구조의 순서는 다음과 같다.
Position(float 변수 x,y,z) - 정점의 좌표로 x,y,z로 값이 구성된다.
RHW(float 변수 rhw) - 동차 좌표계의 w값 이것은 3D좌표계를 사용하는 것이 아닌 2D 좌표계를 사용하여 출력하는것
이 링크를 참고하길 - https://ifyouwanna.tistory.com/entry/rhw-%EA%B0%92%EC%9D%98-%EC%9D%98%EB%AF%B8
Blending Weight Data() - 블린딩 값(이것은 애니메이션을 할 때 사용이 된다)
Vertex Normal(float 변수 x,y,z) - 정점의 법선벡터(노멀벡터라고도 한다) 법선 벡터는 빛 처리를 할때 주로 사용된다.
Vertex Point Size(float 변수 size) - 정점 포인트의 크기
Diffuse Color(DWORD 변수 RGBA값) - RGBA의 값으로 정점의 확산광을 나타낼때 사용
Specular Color(DWORD 변수 RGBA값) - RGBA의 값으로 정점의 반사광을 나타낼때 사용
Texture Coordinate Set1~8(1,2,3, or 4 float) - 텍스쳐의 좌표값(u,v) 최대 8개의 텍스쳐를 겹처서 사용할 수 있기 때문에 8개의 세트가 있으며
첫 번째 값을 diffuse 맵정보로, 두 번째 값을 법선맵 정보로 설정해서 사용하는 식이다

*/
struct CUSTOMVERTEX
{
    float x, y, z, rhw;
    DWORD color;
};

bool bFlat;

//정점 구조 설정 정의
//정점 구조를 미리 매크로로 선언하여 코드를 짧게 쓰도록 하였다.
//개발자가 선언한 정점 구조체가 다음과 같이 이루어져 있다고 컴퓨터에게 알려주는 것(비트 OR연산자를 활용하여 처리한다.)
//현재 아래의 정보는 XYZ 값과 rhw의 값을 가지고 있고 정점의 확산광 값을 갖고 있는다는 것을 정의했다.
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

//각 정점에 값을 초기화 하는 작업
//색깔값을 16진수 말고도 D3DXCOLOR_ARGB와 같은 매크로를 사용해도 된다.
CUSTOMVERTEX vertices[] =
{
    {150.0f,50.0f,0.5f,1.0f,0xffff0000},        //정점1
    {250.0f,250.0f,0.5f,1.0f,0xff00ff00},       //정점2
    {50.0f,250.0f,0.5f,1.0f,0xff00ffff}         //정점3
};

//정점 정보들을 담아둘 버텍스 버퍼
//이것을 하는 이유는 그냥 int, float나 구조체는 모두 RAM에 값이 저장되는데 컴퓨터 그래픽스에서는 방대한 선형 연산을 처리하기 위해
//CPU가 아닌 GPU의 도움을 받아 처리하게 된다.(속도 떄문) 이 떄문에GPU내에 있는 비디오 메모리에 저장하기 위해 버퍼를 만든 것.
LPDIRECT3DVERTEXBUFFER9 g_pVB;

HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    /*  
    
    디바이스에게 정점버퍼를 생성(할당)하는 함수이다.
    인자값 정리
    Length - 할당 받고 싶은 버퍼의 길이(바이트) 현재 정점의 정보를 담을 현재 정점 정보를 담은 변수인 vertices는 3개의 정점 정보를 담고 있기에
    CUSTOMVERTEX의 크기에 3을 곱하여 해당 변수의 크기를 넘겨주었다.
    Usage - 버퍼의 버퍼가 이용되는 방법을 결정하는 몇 가지 부가적인 특성을 지정한다. 이 값에는 부가적인 특성이 없음을 의미하는 0을 지정하거나, 
    다음의 플래그들을 하나 이상 지정할 수 있다

    D3DUSAGE_DYNAMIC >> 버퍼를 동적으로 설정(이것을 지정하지 않을 시 모든 정점들은 정적 버퍼가 된다. 속도는 정점 버퍼가 더 빠르지만 중간에 값 수정이 안된다)
    D3DUSAGE_POINTS >> 버퍼가 포인트 기본형을 보관할 것임을 지정한다.(파티클 시스템 때 이용 및 정점 버퍼만 사용 가능)
    D3DUSAGE_SOFTWAREPROCESSING >> 정점 프로세싱을 소프트웨어로 처리한다
    D3DUSAGE_WRITEONLY >> 어플리케이션이 버퍼에 쓰기만을 수행할 것임을 지정한다
                        >> 이 플래그를 설정하면 드라이버가 쓰기 작업에 최적화된 메모리 위치에 버퍼를 옮길 수 있다
                        >> 이 플래그가 설정된 버퍼에 읽기를 수행하려고 시도하면 오류가 발생

    FVF - 정점 버퍼에 보관될 FVF(위에서 선언한 D3DFVF_CUSTOMVERTEX가 이에 해당한다)
    Pool - 정점 버퍼가 저장될 메모리의 위치(그래픽 카드, 시스템 메모리)와 관리방식 지정
    ppVertexBuffer - (out, retval)반환된 정점 버퍼를 받을 포인터
    pHandle - 이용되지 않는다. Null또는 Nullptr로 설정

    */
    pd3dDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &g_pVB, nullptr);

    //위 작업까지 하는것이 정점 버퍼의 인터페이스를 만든 것이다(틀을 제작한 것)
    //현재 버텍스 버퍼에는 쓰레기 값이 들어가 있음 여기에 정점 값을 넣어줘야함(그 작업이 Lock)

    CUSTOMVERTEX* pVertices;
    //버텍스 버퍼에 메모리 락을 거는 함수
    /*
    
    인자값
    OffsetToLock - Lock을 할 버퍼의 시작점, SizeToLock과 함께 양쪽 모두 0이면 전체 버퍼
    SizeToLock - Lock을 할 바이트의 수, OffsetToLock과 함께 양쪽 모두 0이면 전체 버퍼
    ppbData - (out)읽고 쓸 수 있게 된 메모리 영역의 시작을 가리키는 포인터 (VOID* 포인터)
    Flags - Lock이 이루어지는 방법을 지정,  플래그에 대해서는 ☞ 이쪽 ☜을 참조

    */
    g_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0);

    //memcpy 함수로 pVertices에 vertices값을 복사한다.
    memcpy(pVertices, vertices, sizeof(vertices));

    //메모리 복사가 끝나면 반드시 Unlock을 호출한다.
    g_pVB->Unlock();

    return S_OK;
}

void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;
    V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 45, 50, 170 ), 1.0f, 0 ) );

    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        //출력할 정점 버퍼를 디바이스의 데이터 스트림과 연결시킨다.
        /*
        인자값
        StreamNumber - 다중 스트림을 사용하지 않으면 0을 넣는다.
        pStreamData - 스트림과 연결하고자 하는 버텍스 버퍼의 포인터
        OffsetInBytes - 렌더링 파이프라인으로 공급될 버텍스 데이터의 시작을 지정하는 스트림의 시작 오프셋,
        0이외에 값을 지정하려면 D3DCAPS9 구조체의 D3DDEVCAPS2_STREAMOFFSET 플래그를 확인하여 개발자의 장치가 이를 지원하는지 확인해야한다.
        Stride - 스트림에 연결하고자는 버텍스 버퍼 내 각 요소의 바이트 수(정점 1개의 바이트)

        */

        if (bFlat)
            pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);     //쉐이드 모드를 플랫(직각) 선형 처리를 하지 않는다.
        else
            pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);  //쉐이드 모드를 선형 처리한다.

        if(DXUTWasKeyPressed(VK_SPACE))
            bFlat = !bFlat;

        pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));

        //정점 포맷을 디바이스에 지정한다.
        pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

        /*
        기하 정보를 출력하기 위한 DrawPrimitive() 함수 호출
        인자값
        PrimitiveType - 그리고자 하는 기본형 타입, 삼각현, 선등을 그릴 수 있다.
        StartVertex - 버텍스 읽기를 시작할 버텍스 스트림 요소로의 인덱스, 이 인자는 버텍스 버퍼 내의 데이터를 일부만 그릴 수 있도록 한다.
        PrimitiveCount - 그리고자 하는 기본형의 수(PrimitiveType로 넘긴 인자값)

        정점의 출력순서는 정점을 넣은 기준으로 시계 반대방향으로 출력이 된다.

        https://tadis.tistory.com/entry/DIRECTX-%EB%8F%99%EC%B0%A8%EC%A2%8C%ED%91%9C%EA%B3%84%EC%97%90%EC%84%9C-%EB%B2%84%ED%85%8D%EC%8A%A4-%ED%91%9C%EC%8B%9C
        */
        pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

        V( pd3dDevice->EndScene() );
    }
}

void CALLBACK OnD3D9DestroyDevice( void* pUserContext )
{
    //정점 버퍼는 디바이스가 해제 되기전에 작업해야한다.(프로그램 종료 시)
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


