#include "DXUT.h"
#include "resource.h"

//이 프로젝트에서는 DirectX 내부에 있는 D3DXCreate 함수를 활용하여 도형을 출력하고 
//x파일을 활용하여 메쉬를 불러와 출력하는 예제 프로젝트입니다.
//다음 프로젝트인 Mesh Loder에서 obj파일을 불러와 데이터 파싱을 하여 실제 메쉬를 출력한다.
//https://m.blog.naver.com/PostView.nhn?blogId=aaa4379&logNo=220527229526&proxyReferer=https:%2F%2Fwww.google.com%2F

ID3DXMesh* Objects[5] = { 0, 0, 0, 0, 0 };

int curMesh = 0;

HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	//D3DXCreate~ 함수들은 DX에서 직접 정의된 메쉬 파일들을 생성시키는 함수이다.
	//메쉬 종류는 주전자, 상자, 실린더, 도넛, 구가 있다.
	//기본적으로 인자값에는 디바이스와 메쉬에 대한 정보를 담을 ID3DXMesh*의 변수가 필요하다.


	D3DXCreateTeapot(
		pd3dDevice,
		&Objects[0],
		0);//0을 지정

	D3DXCreateBox(
		pd3dDevice,
		3.0f, // 너비
		3.0f, // 높이
		3.0f, // 깊이
		&Objects[1],
		0);

	D3DXCreateCylinder(
		pd3dDevice,
		2.0f, // z 축의 부의 옆의 면의 반경(직접 숫자 바꿔보면 암)
		2.0f, // z 축의 정의 옆의 면의 반경(직접 숫자 바꿔보면 암)
		4.0f, // 실린더의 길이
		10,   // 실린더 옆면의 폴리곤 수(최소 3개 이상)
		10,   // 실린더 둘레의 면 갯수
		&Objects[2],
		0);

	D3DXCreateTorus(
		pd3dDevice,
		1.0f, // 안쪽 둘레
		3.0f, // 바깥쪽 둘레
		10,   // 둘레의 면 수(많을 수록 굵기가 동그래짐)
		10,   // 원형 폴리곤의 수(많을 수록 원에 가까워짐)
		&Objects[3],
		0);

	D3DXCreateSphere(
		pd3dDevice,
		3.0f, // 지름
		10,   // 원 반경에 대한 폴리곤 수
		4,   // 원의 지름에 대한 폴리곤 수
		&Objects[4],
		0);

	//이 내용은 추후 Camera 프로젝트에서 참고
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f, // 90 - degree
		(float)640 / (float)480,
		1.0f,
		1000.0f);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, &proj);
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    return S_OK;
}

void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	if (DXUTWasKeyPressed(VK_LEFT))
		curMesh--;

	if (DXUTWasKeyPressed(VK_RIGHT))
		curMesh++;

	if (curMesh > 4)
		curMesh = 4;

	if (curMesh < 0)
		curMesh = 0;
}

void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;
    V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 45, 50, 170 ), 1.0f, 0 ) );

    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		//이 내용은 추후 Camera 프로젝트에서 참고
		static float angle = (3.0f * D3DX_PI) / 2.0f;
		static float cameraHeight = 0.0f;
		static float cameraHeightDirection = 5.0f;

		D3DXVECTOR3 position(cosf(angle) * 10.0f, cameraHeight, sinf(angle) * 10.0f);

		// the camera is targetted at the origin of the world
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);

		// the worlds up vector
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		pd3dDevice->SetTransform(D3DTS_VIEW, &V);

		// compute the position for the next frame
		angle += fElapsedTime;
		if (angle >= 6.28f)
			angle = 0.0f;

		// compute the height of the camera for the next frame
		cameraHeight += cameraHeightDirection * fElapsedTime;
		if (cameraHeight >= 10.0f)
			cameraHeightDirection = -5.0f;

		if (cameraHeight <= -10.0f)
			cameraHeightDirection = 5.0f;


		//매쉬를 그려주는 함수
		//인자 값으로 넘긴 subSet값으로 현재 그릴 메쉬를 지정한다.(기본적으로 0으로 설정한다.)
		//이 매쉬에 매터리얼이나 텍스쳐를 n개를 입혀 그에 해당하는 텍스쳐랑 메터리얼의 해당하는 인덱스를 넘겨주면 된다.)
		Objects[curMesh]->DrawSubset(0);


        V( pd3dDevice->EndScene() );
    }
}

void CALLBACK OnD3D9DestroyDevice( void* pUserContext )
{
	for (int i = 0; i < 5; i++)
	{
		Objects[i]->Release();
	}
}


INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    DXUTSetCallbackD3D9FrameRender( OnD3D9FrameRender );
    DXUTSetCallbackD3D9DeviceDestroyed( OnD3D9DestroyDevice );
    DXUTSetCallbackFrameMove( OnFrameMove );

    DXUTInit( true, true );
    DXUTSetHotkeyHandling( true, true, true );
    DXUTSetCursorSettings( true, true );
    DXUTCreateWindow( L"Emptyproject" );
    DXUTCreateDevice( true, 640, 480 );

    DXUTMainLoop();

    return DXUTGetExitCode();
}


