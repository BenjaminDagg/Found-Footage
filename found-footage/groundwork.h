#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <io.h>
#include "resource.h"
#include <stack>
#include <queue>

using namespace std;

bool LoadOBJ(char * filename, ID3D11Device * g_pd3dDevice, ID3D11Buffer ** ppVertexBuffer, int * vertex_count);
bool Load3DS(char *filename, ID3D11Device* g_pd3dDevice, ID3D11Buffer **ppVertexBuffer, int *vertex_count);
float Vec3Length(const XMFLOAT3 &v);
float Vec3Dot(XMFLOAT3 a, XMFLOAT3 b);
XMFLOAT3 Vec3Cross(XMFLOAT3 a, XMFLOAT3 b);
XMFLOAT3 Vec3Normalize(const  XMFLOAT3 &a);


class node
{
public:
	//setup:
	vector<node*> neighbors;
	vector<int> weights;
	XMFLOAT3 pos;

	void init_connection(node* neighbor, int weight)
	{
		neighbors.push_back(neighbor);
		weights.push_back(weight);
	}
	void init_double_connection(node* neighbor, int weight)
	{
		neighbors.push_back(neighbor);
		weights.push_back(weight);
		neighbor->neighbors.push_back(this);
		neighbor->weights.push_back(weight);
	}
	//for algorithm:
	node* comming_from;
	int current_weight;
	bool calculated;
	node()
	{
		current_weight = 100000000;//for infinite
		comming_from = NULL;
		calculated = 0;
		pos = XMFLOAT3(0, 0, 0);
	}


};



/* class for 3d objects like chairs, tables, etc.
Holds position, scale, rotation*/
class Object {
public:
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	float scale;

	//defualt
	Object() {
		this->pos = XMFLOAT3(0, 0, 0);
		this->rot = XMFLOAT3(0, 0, 0);
		this->scale = 1.0;
	}

	//constructor to set all fields
	Object(XMFLOAT3 pos, XMFLOAT3 rot, float scale) {
		this->pos = pos;
		this->rot = rot;
		this->scale = scale;
	}

	//keeps player from colliding with object
	bool playerCollided(XMFLOAT3 playerPos) {
		playerPos.x = -playerPos.x;
		playerPos.y = -playerPos.y;
		playerPos.z = -playerPos.z - 5;


		float dist = sqrt(
			pow(playerPos.x - pos.x, 2) + 
			pow(playerPos.z  - (pos.y * -1)   , 2) );

		if (dist < 2.0)
			return true;
		else
			return false;
		
	}

	XMMATRIX get_matrix(XMMATRIX &view) {
		XMMATRIX T, Rx, Ry, S, V, M;

		//translation matrix
		T = XMMatrixTranslation(pos.x, pos.y, pos.z);

		//x rotation
		Rx = XMMatrixRotationX(rot.x);

		//y rotation
		Ry = XMMatrixRotationY(rot.y);

		XMMATRIX Rz = XMMatrixRotationZ(rot.z);

		//scale
		S = XMMatrixScaling(scale, scale, scale);

		//view matrix;
		V = view;

		//resulting matrix
		M = S * Rx * Ry * Rz * T;

		return M;



	}
};



struct SimpleVertex
	{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
	XMFLOAT3 Norm;
	};


struct ConstantBuffer
	{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;
	int green;
	float green_col;
	};
//*****************************************
class bitmap
	{

	public:
		BYTE *image;
		int array_size;
		BITMAPFILEHEADER bmfh;
		BITMAPINFOHEADER bmih;
		bitmap()
			{
			image = NULL;
			}
		~bitmap()
			{
			if(image)
				delete[] image;
			array_size = 0;
			}
		bool read_image(char *filename)
			{
			ifstream bmpfile(filename, ios::in | ios::binary);
			if (!bmpfile.is_open()) return FALSE;	// Error opening file
			bmpfile.read((char*)&bmfh, sizeof(BITMAPFILEHEADER));
			bmpfile.read((char*)&bmih, sizeof(BITMAPINFOHEADER));
			bmpfile.seekg(bmfh.bfOffBits, ios::beg);
			//make the array
			if (image)delete[] image;
			int size = bmih.biWidth*bmih.biHeight * 3;
			image = new BYTE[size];//3 because red, green and blue, each one byte
			bmpfile.read((char*)image,size);
			array_size = size;
			bmpfile.close();
			check_save();
			return TRUE;
			}
		BYTE get_pixel(int x, int y,int color_offset) //color_offset = 0,1 or 2 for red, green and blue
			{
			int array_position = x*3 + y* bmih.biWidth*3+ color_offset;
			if (array_position >= array_size) return 0;
			if (array_position < 0) return 0;
			return image[array_position];
			}
		void check_save()
			{
			ofstream nbmpfile("newpic.bmp", ios::out | ios::binary);
			if (!nbmpfile.is_open()) return;
			nbmpfile.write((char*)&bmfh, sizeof(BITMAPFILEHEADER));
			nbmpfile.write((char*)&bmih, sizeof(BITMAPINFOHEADER));
			//offset:
			int rest = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
			if (rest > 0)
				{
				BYTE *r = new BYTE[rest];
				memset(r, 0, rest);
				nbmpfile.write((char*)&r, rest);
				}
			nbmpfile.write((char*)image, array_size);
			nbmpfile.close();

			}
	};
class billboard
{
public:
	billboard()
	{
		position = XMFLOAT3(0, 0, 0);
		scale = 1;
		transparency = 1;
		impulse = XMFLOAT3(0, 0, 1);

		strafe = XMFLOAT3(1, 0, 0);
		prevDist = 0.0;
		draw = true;
		curr = NULL;
		next = NULL;

		
		

	}
	XMFLOAT3 position; //obvious
	float scale;		//in case it can grow
	float transparency; //for later use
	XMFLOAT3 impulse;
	XMFLOAT3 rotation;
	float prevDist;
	bool draw;
	XMFLOAT3 strafe;
	node *curr;
	node *next;

	bool atClosest(vector<node*> &list, XMFLOAT3 destPos) {
		int playerInd = -1;
		int myClosest;
		int min = 10000000000000;

		//find my player node
		for (int i = 0; i < list.size(); i++) {
			if (sqrt(pow(list[i]->pos.x - destPos.x, 2) + pow(list[i]->pos.z - destPos.z, 2)) < min) {
				min = sqrt(pow(list[i]->pos.x - destPos.x, 2) + pow(list[i]->pos.z - destPos.z, 2));
				playerInd = i;
			}
		}
		min = 1000000000000;
		//find my node
		for (int i = 0; i < list.size(); i++) {
			if (list[i]->pos.x == curr->pos.x && list[i]->pos.z == curr->pos.z)
				myClosest = i;
		}

		return myClosest == playerInd;
	}

	node* get_closest(vector<node*> &list) {
		float dis;
		float min = 100000000000000.0;
		int index = -1;

		for (int i = 0; i < list.size(); i++) {
			dis = sqrt(pow(position.x - list[i]->pos.x, 2) + pow(position.z - list[i]->pos.z, 2));

			if (dis < min) {
				min = dis;
				index = i;
			}
		}
		curr = list[index];
		return list[index];
	}

	bool no_path(vector<node*> list, XMFLOAT3 ppos) {

		//H
		XMFLOAT3 target1 = XMFLOAT3(18, -0.75, 45);
		//D
		XMFLOAT3 target2 = XMFLOAT3(28, -0.75, 24);

		//if im at H and player is around D
		if (
			sqrt(pow(position.x - target1.x, 2) + pow(position.z - target1.z, 2)) <= 4.0 &&
			sqrt(pow(ppos.x - target2.x, 2) + pow(ppos.z - target2.z, 2)) <= 4.0
			)
			return true;
		//if im at D and the player is at H
		else if (
			sqrt(pow(position.x - target2.x, 2) + pow(position.z - target2.z, 2)) <= 4.0 &&
			sqrt(pow(ppos.x - target1.x, 2) + pow(ppos.z - target1.z, 2)) <= 4.0
			)
			return true;
		else
			return false;
	}


	void find_next(vector<node*> list, node *dest, XMFLOAT3 destPos) {

		int no_path_index = -1;
		float min = 1000000000000;
		int minIndex = -1;

		destPos.x = -destPos.x;
		destPos.y = -destPos.y;
		destPos.z = -destPos.z - 5;

		
		
		//if enemy already at players node then do nothing
		if (atClosest(list, destPos)) {
			curr = curr;
			next = curr;
			return;
		}
		else if (no_path(list, destPos)) {
			position = XMFLOAT3(0.5, -.75, 11.5);
			for (int i = 0; i < list.size(); i++) {
				if (list[i]->pos.x == 0.5 && list[i]->pos.z == 11.5)
					no_path_index = i;
			}
			curr = list[no_path_index];

			for (int i = 0; i < curr->neighbors.size(); i++) {
				float d = sqrt(pow(curr->neighbors[i]->pos.x - destPos.x, 2) + pow(curr->neighbors[i]->pos.z - destPos.z, 2));

				if (d < min) {
					min = d;
					minIndex = i;
				}

			}

			next = curr->neighbors[minIndex];
			return;
		}
		

		get_closest(list);

		for (int i = 0; i < curr->neighbors.size(); i++) {
			float d = sqrt(pow(curr->neighbors[i]->pos.x - destPos.x, 2) + pow(curr->neighbors[i]->pos.z - destPos.z, 2));

			if (d < min) {
				min = d;
				minIndex = i;
			}
			
		}

		next = curr->neighbors[minIndex];

	}

	void go_to_next() {

		XMVECTOR billVector = XMLoadFloat3(&position);

		
		XMVECTOR playerVector = XMLoadFloat3(&next->pos);

		//direction from billboard to player
		XMVECTOR dir = playerVector - billVector;

		//direction that the billboard will face
		XMFLOAT3 forward;

		playerVector = XMVector3Normalize(playerVector);
		billVector = XMVector3Normalize(billVector);


		XMStoreFloat3(&forward, dir);

		//move billboard towards camera
		position.x += forward.x * .003;
		position.y += forward.y * .003;
		position.z += forward.z * .003;

	}

	bool atNext() {
		float d = sqrt(pow(position.x - next->pos.x, 2) + pow(position.z - next->pos.z,2));
		
		if (d < 1) {
			curr = next;
			next == NULL;
			return true;
		}
		else
			return false;

	}

	
	void pathfind(vector<node*> &list, node *dest, XMFLOAT3 pos) {

		if (next == NULL) {
			find_next(list, dest, pos);
			go_to_next();
		}
		else if (atNext()) {
			find_next(list, dest,pos);
			go_to_next();
		}
		else {
			go_to_next();
		}

	}
	

	XMMATRIX get_matrix(XMMATRIX &ViewMatrix)
	{
		XMMATRIX view, R, T, S;
		view = ViewMatrix;
		//eliminate camera translation:
		view._41 = view._42 = view._43 = 0.0;
		XMVECTOR det;
		R = XMMatrixInverse(&det, view);//inverse rotation
		T = XMMatrixTranslation(position.x, position.y, position.z);
		S = XMMatrixScaling(scale, scale, scale);
		return S*R*T;
	}


	XMMATRIX get_matrix_no_rotation() {
		XMMATRIX T, Rx, S;

		T = XMMatrixTranslation(position.x, position.y, position.z);
		Rx = XMMatrixRotationX(XM_PI / 2);
		S = XMMatrixScaling(scale, scale, scale);

		return S * Rx * T;

	}

	/*calculates this billboards distance to the player*/
	float distanceToPlayer(XMFLOAT3 me, XMFLOAT3 player) {
		float distance = sqrt(
			pow(me.x - player.x, 2) +
			pow(me.z - player.z, 2)
			);

		return distance;
	}

	/*detects if billboard collided with wall by checking its location on the bitmap*/
	bool collided(bitmap *leveldata, XMFLOAT3 nextPos,XMFLOAT3 playerPos) {
		
		
	
		//49.885, 1.425

		//higher the x the more it gets pushed left
		//higher the z the closer forward it goes
		
		BYTE col = leveldata->get_pixel(((nextPos.x / 2) + 49.885), ((nextPos.z / 2) - 1.425), 0);
		if (col == 0) {
			return false;
		}
		else
			return true;
		

	    /*
		BYTE col = leveldata->get_pixel(((nextPos.x / 2) + 49.885), ((nextPos.z / 2 - 1.425)), 0);
		BYTE col2 = leveldata->get_pixel((((nextPos.x + 0.5) / 2) + 49.885), ((nextPos.z / 2) - 1.425), 0);
		BYTE col3 = leveldata->get_pixel((((nextPos.x - .5) / 2) + 49.885), ((nextPos.z / 2) - 1.425), 0);
		if (col == 0 && col2 == 0 && col3 == 0) {
			return true;
		}
		else
			return false;
		*/
		
		
	}

	/*checks if the enemy has a view (line of sight) of the player by checking
	each coordinate on the path from the billboards starting posiion to the players
	position. if any of the coordinates on tthe path collide with a wall then there
	is a wall in the way of the player and enemy so the enemy cant move yet. If no collisions
	found then the enemy has a view of the player not obstructed by any walls so they are
	good to move forward.*/
	bool hasLineOfSight(XMFLOAT3 enemyPos, XMMATRIX &viewm, bitmap *leveldata) {


		return true;

	}




	/*updates enemy to move toward player when they have a line of sight to them and the pah has
	no walls*/
	void forward(XMMATRIX &viewm, XMFLOAT3 playerPos, bitmap * leveldata) {
		//speed of billbords
		float speed = .003;

		//original position before moving forward
		//if new position is inside a wall then go back
		//to original
		XMFLOAT3 origPos = position;

		//billboard vector
		XMVECTOR billVector = XMLoadFloat3(&position);

		//player vector
		playerPos.x = -playerPos.x;
		playerPos.y = -playerPos.y;
		playerPos.z = -playerPos.z - 5;
		XMVECTOR playerVector = XMLoadFloat3(&playerPos);

		//direction from billboard to player
		XMVECTOR dir = playerVector - billVector;

		//direction that the billboard will face
		XMFLOAT3 forward;

		playerVector = XMVector3Normalize(playerVector);
		billVector = XMVector3Normalize(billVector);


		XMStoreFloat3(&forward, dir);

		//move billboard towards camera
		position.x += forward.x * speed;
		position.y += forward.y * speed;
		position.z += forward.z * speed;

		//if billboard collided then go back to original positoin
		if (collided(leveldata, position,playerPos)) {
			position = origPos;

		}



	}


	






};
////////////////////////////////////////////////////////////////////////////////
//lets assume a wall is 10/10 big!
#define FULLWALL 2
#define HALFWALL 1
class wall
	{
	public:
		XMFLOAT3 position;
			int texture_no;
			int rotation; //0,1,2,3,4,5 ... facing to z, x, -z, -x, y, -y
			wall()
				{
				texture_no = 0;
				rotation = 0;
				position = XMFLOAT3(0,0,0);
				}
			XMMATRIX get_matrix()
				{
				XMMATRIX R, T, T_offset;				
				R = XMMatrixIdentity();
				T_offset = XMMatrixTranslation(0, 0, -HALFWALL);
				T = XMMatrixTranslation(position.x, position.y, position.z);
				switch (rotation)//0,1,2,3,4,5 ... facing to z, x, -z, -x, y, -y
					{
						default:
						case 0:	R = XMMatrixRotationY(XM_PI);		T_offset = XMMatrixTranslation(0, 0, HALFWALL); break;
						case 1: R = XMMatrixRotationY(XM_PIDIV2);	T_offset = XMMatrixTranslation(0, 0, HALFWALL); break;
						case 2:										T_offset = XMMatrixTranslation(0, 0, HALFWALL); break;
						case 3: R = XMMatrixRotationY(-XM_PIDIV2);	T_offset = XMMatrixTranslation(0, 0, HALFWALL); break;
						case 4: R = XMMatrixRotationX(XM_PIDIV2);	T_offset = XMMatrixTranslation(0, 0, -HALFWALL); break;
						case 5: R = XMMatrixRotationX(-XM_PIDIV2);	T_offset = XMMatrixTranslation(0, 0, -HALFWALL); break;
					}
				return T_offset * R * T;
				}
	};
//********************************************************************************************
class level
	{
	private:
		bitmap leveldata;	//still private *NEW*
		vector<wall*> walls;						//all wall positions
		vector<ID3D11ShaderResourceView*> textures;	//all wall textures
		void process_level()
			{
			//we have to get the level to the middle:
			int x_offset = (leveldata.bmih.biWidth/2)*FULLWALL;

			//lets go over each pixel without the borders!, only the inner ones
			for (int yy = 1; yy < (leveldata.bmih.biHeight - 1);yy++)
				for (int xx = 1; xx < (leveldata.bmih.biWidth - 1); xx++)
					{
					//wall information is the interface between pixels:
					//blue to something not blue: wall. texture number = 255 - blue
					//green only: floor. texture number = 255 - green
					//red only: ceiling. texture number = 255 - red
					//green and red: floor and ceiling ............
					BYTE red, green, blue;

					blue = leveldata.get_pixel(xx, yy, 0);
					green = leveldata.get_pixel(xx, yy, 1);
					red = leveldata.get_pixel(xx, yy, 2);
					
					if (blue > 0)//wall possible
						{
						int texno = 255 - blue;
						BYTE left_red = leveldata.get_pixel(xx - 1, yy, 2);
						BYTE left_green = leveldata.get_pixel(xx - 1, yy, 1);
						BYTE right_red = leveldata.get_pixel(xx + 1, yy, 2);
						BYTE right_green = leveldata.get_pixel(xx + 1, yy, 1);
						BYTE top_red = leveldata.get_pixel(xx, yy+1, 2);
						BYTE top_green = leveldata.get_pixel(xx, yy+1, 1);
						BYTE bottom_red = leveldata.get_pixel(xx, yy-1, 2);
						BYTE bottom_green = leveldata.get_pixel(xx, yy-1, 1);

						if (left_red>0 || left_green > 0)//to the left
							init_wall(XMFLOAT3(xx*FULLWALL - x_offset, 0, yy*FULLWALL), 3, texno);
						if (right_red>0 || right_green > 0)//to the right
							init_wall(XMFLOAT3(xx*FULLWALL - x_offset, 0, yy*FULLWALL), 1, texno);
						if (top_red>0 || top_green > 0)//to the top
							init_wall(XMFLOAT3(xx*FULLWALL - x_offset, 0, yy*FULLWALL), 2, texno);
						if (bottom_red>0 || bottom_green > 0)//to the bottom
							init_wall(XMFLOAT3(xx*FULLWALL - x_offset, 0, yy*FULLWALL), 0, texno);
						}
					if (red > 0)//ceiling
						{
						int texno = 255 - red;
						init_wall(XMFLOAT3(xx*FULLWALL - x_offset, 0,yy*FULLWALL), 5, texno);
						}
					if (green > 0)//floor
						{
						int texno = 255 - green;
						init_wall(XMFLOAT3(xx*FULLWALL - x_offset, 0,yy*FULLWALL), 4, texno);
						}
					}
			}
		void init_wall(XMFLOAT3 pos, int rotation, int texture_no)
			{
			wall *w = new wall;
			walls.push_back(w);
			w->position = pos;
			w->rotation = rotation;
			w->texture_no = texture_no;
			}
	public:
		bitmap *get_bitmap()//get method *NEW*
			{
			return &leveldata;
			}
		level()
			{
			}
		void init(char *level_bitmap)
			{
			if(!leveldata.read_image(level_bitmap))return;
			process_level();
			}
		bool init_texture(ID3D11Device* pd3dDevice,LPCWSTR filename)
			{
			// Load the Texture
			ID3D11ShaderResourceView *texture;
			HRESULT hr = D3DX11CreateShaderResourceViewFromFile(pd3dDevice, filename, NULL, NULL, &texture, NULL);
			if (FAILED(hr))
				return FALSE;
			textures.push_back(texture);
			return TRUE;
			}
		ID3D11ShaderResourceView *get_texture(int no)
			{
			if (no < 0 || no >= textures.size()) return NULL;
			return textures[no];
			}
		XMMATRIX get_wall_matrix(int no)
			{
			if (no < 0 || no >= walls.size()) return XMMatrixIdentity();
			return walls[no]->get_matrix();
			}
		int get_wall_count()
			{
			return walls.size();
			}
		void render_level(ID3D11DeviceContext* ImmediateContext,ID3D11Buffer *vertexbuffer_wall,XMMATRIX *view, XMMATRIX *projection, ID3D11Buffer* dx_cbuffer)
			{
			//set up everything for the waqlls/floors/ceilings:
			UINT stride = sizeof(SimpleVertex);
			UINT offset = 0;			
			ImmediateContext->IASetVertexBuffers(0, 1, &vertexbuffer_wall, &stride, &offset);
			ConstantBuffer constantbuffer;			
			constantbuffer.View = XMMatrixTranspose(*view);
			constantbuffer.Projection = XMMatrixTranspose(*projection);			
			XMMATRIX wall_matrix,S;
			ID3D11ShaderResourceView* tex;
			//S = XMMatrixScaling(FULLWALL, FULLWALL, FULLWALL);
			S = XMMatrixScaling(1, 1, 1);
			for (int ii = 0; ii < walls.size(); ii++)
				{
				wall_matrix = walls[ii]->get_matrix();
				int texno = walls[ii]->texture_no;
				if (texno >= textures.size())
					texno = 0;
				tex = textures[texno];
				wall_matrix = wall_matrix;// *S;

				constantbuffer.World = XMMatrixTranspose(wall_matrix);
				
				ImmediateContext->UpdateSubresource(dx_cbuffer, 0, NULL, &constantbuffer, 0, 0);
				ImmediateContext->VSSetConstantBuffers(0, 1, &dx_cbuffer);
				ImmediateContext->PSSetConstantBuffers(0, 1, &dx_cbuffer);
				ImmediateContext->PSSetShaderResources(0, 1, &tex);
				ImmediateContext->Draw(6, 0);
				}
			}
	};

	class Tape {
	public:
		XMFLOAT3 pos;
		XMFLOAT3 rot;
		float scale;
		float pickup_radius; //radius that the player must be in to pick up tape
		bool inRadius;	//flag for when player is in pickup radius of the tape
		bool pickedUp;	//flag for if the tape was picked up
		bool collided;
		bool completed;

						//defualt
		Tape() {
			this->pos = XMFLOAT3(0, 0, 0);
			this->rot = XMFLOAT3(0, 0, 0);
			this->scale = 1.0;
			this->pickup_radius = 1.5;
			this->inRadius = false;
			this->pickedUp = false;
			this->collided = false;
			this->completed = false;
		}

		//constructor to set all fields
		Tape(XMFLOAT3 pos, XMFLOAT3 rot, float scale) {
			this->pos = pos;
			this->rot = rot;
			this->scale = scale;
			this->pickup_radius = 1.5;
			this->inRadius = false;
			this->pickedUp = false;
			this->collided = false;
			this->completed = false;
		}

		//constinuously checks if player is in pickup radius and updates inRadius
		void update(XMFLOAT3 playerPos) {
			playerPos.x = -playerPos.x;
			playerPos.y = -playerPos.y;
			playerPos.z = -playerPos.z - 5;


			float dist = sqrt(
				pow(playerPos.x - pos.x, 2) +
				pow(playerPos.z - pos.z, 2));

			if (dist < pickup_radius) {
				inRadius = true;
			}
			else {
				inRadius = false;
			}
		}

		
		//keeps player from walking through tape
		bool playerCollided(XMFLOAT3 playerPos) {
			playerPos.x = -playerPos.x;
			playerPos.y = -playerPos.y;
			playerPos.z = -playerPos.z - 5;


			float dist = sqrt(
				pow(playerPos.x - pos.x, 2) +
				pow(playerPos.y - pos.y, 2) +
				pow(playerPos.z - pos.z, 2));

			if (dist < pickup_radius && !pickedUp) {
				inRadius = true;
				return true;
			}
			else {
				inRadius = false;
				return false;
			}


		}

		//keeps tape from clipping through wall
		bool wallCollided(bitmap *leveldata, XMFLOAT3 pos) {

			BYTE col = leveldata->get_pixel(((-pos.x / 2) + 50.5), ((-pos.z / 2) - 2.5), 0);
			BYTE col2 = leveldata->get_pixel(((-(pos.x + 1.0) / 2) + 50.5), ((-pos.z / 2) - 2.5), 0);
			BYTE col3 = leveldata->get_pixel(((-(pos.x - 1.0)/ 2) + 50.5), ((-pos.z / 2) - 2.5), 0);
			
			if (col == 0 && col2 == 0 && col3 == 0) {
				return false;
			}
		
			else
				return true;

		}

		/* drops the tape if it is held by finding the forward direction and moving it in that direction */
		void drop(XMFLOAT3 camPos, float r, bitmap *leveldata) {

			XMMATRIX R = XMMatrixRotationY(-r);
			XMFLOAT3 drop_pos;
			XMFLOAT3 origPos = pos;

			//find forward direction
			XMFLOAT3 forward = XMFLOAT3(0, 0, 1);
			XMVECTOR f = XMLoadFloat3(&forward);
			f = XMVector3TransformCoord(f, R);
			XMStoreFloat3(&forward, f);

			//set new position
			pos.y = -.75;	//set y on the floor level
			pos.x += forward.x * 1.75;
			pos.z += forward.z * 1.75;

			//set drop pos to the new pos to test for collision
			drop_pos = pos;

			//if the new position is inside of a wall then move it back a little
			if (wallCollided(leveldata, drop_pos)) {
				pos.x = origPos.x;
				pos.y = origPos.y;
				pos.x += forward.x * 1.5;
				pos.z += forward.z * 1.25;
			}

			/* ====== drop zone ========*/
			//right = -16.5
			//lefft = -24
			//bottom = 13.25
			//top = 21

			if (pos.x >= -26 && pos.x <= -16.5 && pos.z >= 13.25 && pos.z <= 21)
				completed = true;

			//set pickedup to false
			pickedUp = false;


		}


		XMMATRIX get_matrix(XMFLOAT3 playerPos, XMMATRIX &view, float r,bitmap *leveldata) {

			XMMATRIX T, T2, T3, Rx, Ry, S, V, M, Rx90, Rc, Tc, Tt;

			//soecail case if the tape is picked up make it fallow the camera
			if (pickedUp) {

				XMFLOAT3 origPos = pos;
				XMFLOAT3 pPos = playerPos;
				
				pPos.z = (playerPos.z - 5.0) + 1.0;
	
				
				playerPos.x = -playerPos.x;
				playerPos.y = -playerPos.y;
				playerPos.z = -playerPos.z - 5;

				pos.x = playerPos.x;
				pos.y = -0.3;
				pos.z = playerPos.z;

					
				if (wallCollided(leveldata,pPos)) {
					pos = origPos;
					
				}
				else if (wallCollided(leveldata, pPos)) {
					pos = origPos;
				}
			
				S = XMMatrixScaling(scale, scale, scale);

				T = XMMatrixTranslation(pos.x, pos.y, pos.z);

				T2 = XMMatrixTranslation(0, 0, 1);
				T3 = XMMatrixTranslation(0, 0, -1);

				Rx90 = XMMatrixRotationX(-XM_PI / 2);

				Ry = XMMatrixRotationY(-r);
				rot.y = -r;

				


				XMVECTOR det;

				return S * Rx90 * T2 * Ry * T3 * T;


			}



			//tape model is sidewys by default so rotate it 90 degrees
			Rx90 = XMMatrixRotationX(-XM_PI / 2);

			//translation matrix
			T = XMMatrixTranslation(pos.x, pos.y, pos.z);

			//x rotation
			Rx = XMMatrixRotationX(rot.x);

			//y rotation

			Ry = XMMatrixRotationY(rot.y);

			T2 = XMMatrixTranslation(0, 0, 1);
			T3 = XMMatrixTranslation(0, 0, -1);


			//scale
			S = XMMatrixScaling(scale, scale, scale);



			//resulting matrix
			M = S * Rx90 * T2 * Ry * T3 * T;

			return M;



		}
	};

	class camera
		{
		private:

		public:
			int w, s, a, d;
			XMFLOAT3 position;
			XMFLOAT3 rotation;
			XMFLOAT3 pos_cam_pos;
			camera()
				{
				w = s = a = d = 0;
				position = pos_cam_pos = XMFLOAT3(0, 0, 0);
				}

			void animation(bitmap *leveldata, Object *obj, int numObj, Tape *tape)//bitmap in the argument NOW YOU CAN USE IT HERE!!! *NEW*
				{
				XMMATRIX R, T;
				R = XMMatrixRotationY(-rotation.y);

				XMFLOAT3 forward = XMFLOAT3(0, 0, 1);
				XMVECTOR f = XMLoadFloat3(&forward);
				f = XMVector3TransformCoord(f, R);
				XMStoreFloat3(&forward, f);
				XMFLOAT3 side = XMFLOAT3(1, 0, 0);
				XMVECTOR si = XMLoadFloat3(&side);
				si = XMVector3TransformCoord(si, R);
				XMStoreFloat3(&side, si);
				pos_cam_pos = position;
				if (w)
					{
					pos_cam_pos.x -= forward.x * 0.15;
					pos_cam_pos.y -= forward.y * 0.15;
					pos_cam_pos.z -= forward.z * 0.15;
					}
				if (s)
					{
					pos_cam_pos.x += forward.x * 0.15;
					pos_cam_pos.y += forward.y * 0.15;
					pos_cam_pos.z += forward.z * 0.15;
					}
				/*if (d)
					{
					position.x -= side.x * 0.01;
					position.y -= side.y * 0.01;
					position.z -= side.z * 0.01;
					}
				if (a)
					{
					position.x += side.x * 0.01;
					position.y += side.y * 0.01;
					position.z += side.z * 0.01;
					}
					*/
				if (d)
					{
					rotation.y-=0.005;
					}
				if (a)
					{
					rotation.y += 0.005;
					}
				
				BYTE col = leveldata->get_pixel(((-pos_cam_pos.x / 2) + 50.5), ((-pos_cam_pos.z / 2) - 2.5), 0);
				if (col == 0 && obj[0].playerCollided(pos_cam_pos) == false && !tape[0].playerCollided(pos_cam_pos)) {
					bool flag = false;

					for (int i = 0; i < numObj; i++) {
						if (obj[i].playerCollided(pos_cam_pos))
							flag = true;
					}
					if (!flag) {
						tape[0].collided = false;
						position.x = pos_cam_pos.x;
						position.y = pos_cam_pos.y;
						position.z = pos_cam_pos.z;
					}
				}
				
				else {
					tape[0].collided = true;
				}
			
				
				
				
				}
			XMMATRIX get_matrix(XMMATRIX *view)
				{
				XMMATRIX R, T;
				R = XMMatrixRotationY(rotation.y);
				T = XMMatrixTranslation(position.x, position.y, position.z);
				return T*(*view)*R;
				}

			node* get_closest(vector<node*> &list) {
				float dis;
				float min = 100000000000000.0;
				int index = -1;

				for (int i = 0; i < list.size(); i++) {
					dis = sqrt(pow(position.x - list[i]->pos.x, 2) + pow(position.z - list[i]->pos.z,2));

					if (dis < min) {
						min = dis;
						index = i;
					}
				}

				return list[index];
			}

			
		};


	/* this class mainly holds state info of the game like if you are alreadt holding an object,
	how many objects you have completed already*/
	class Player {
	public:
		bool holdingTape;
		int completedTapes;	//number of tapes takes to objective
		Tape *heldTape;	//pointer to held tape so you can modify it easier

		Player() {
			holdingTape = false;
			completedTapes = 0;
			heldTape = new Tape();
			heldTape = NULL;
		}
	};

	class Arrow {
	public:
		XMFLOAT3 pos;
		XMFLOAT3 rot;
		float scale;
		XMFLOAT3 closest_tape;

		Arrow() {
			pos = XMFLOAT3(0, 0, 0);
			rot = XMFLOAT3(0, 0, 0);
			scale = 1.0;
			closest_tape = XMFLOAT3(0, 0, 0);
		}

		void find_closest(Tape t[]) {
			
			float min = 1000000000000.0;
			int index = -1;
			float d;
			vector<int> excluded;
			
			
			for (int i = 0; i < 5; i++) {
				d = sqrt(
					pow(pos.x - t[i].pos.x, 2) +
					pow(pos.z - t[i].pos.z, 2)
					);
				if (d < min && !t[i].pickedUp && !t[i].completed) {
					min = d;
					index = i;
				}
			}

			if (index == -1)
				closest_tape = XMFLOAT3(0,0,0);
			else {
				closest_tape.x = t[index].pos.x;
				closest_tape.y = t[index].pos.y;
				closest_tape.z = t[index].pos.z;
			}
			
			
		}

		XMMATRIX get_matrix(XMFLOAT3 playerPos, XMMATRIX &view, float r) {

			

			XMMATRIX T, T2, T3, Rx, Ry, S, V, M, Rx90, Rc, Tc, Tt;

	

			//soecail case if the tape is picked up make it fallow the camera
			scale = 0.34;
				Rx = XMMatrixRotationX(-XM_PI / 6);

				XMFLOAT3 origPos = pos;
				XMFLOAT3 pPos = playerPos;

				pPos.z = (playerPos.z - 5.0) + 1.0;


				playerPos.x = -playerPos.x;
				playerPos.y = -playerPos.y;
				playerPos.z = -playerPos.z - 5;

				pos.x = playerPos.x;
				pos.y = 0.1;
				pos.z = playerPos.z;

				XMVECTOR myVector = XMLoadFloat3(&pos);


				XMVECTOR tapeVector = XMLoadFloat3(&closest_tape);

				//direction from billboard to player
				XMVECTOR dir = myVector - tapeVector;
				XMFLOAT3 to = XMFLOAT3(0,0,1);
				XMStoreFloat3(&to, dir);

				XMVECTOR up = XMVectorSet(0, 1, 0, 0);
				XMVECTOR towards = XMVectorSet(to.x,to.y,to.z,0);

				XMVECTOR ortho = XMVector3Cross(up, XMVector3Normalize(towards));
				XMVECTOR angle = XMVector3Dot(up, XMVector3Normalize(towards));
				angle *= 5;
				XMMATRIX localRot = XMMatrixRotationAxis(ortho, XMVectorGetX(angle));
				

				S = XMMatrixScaling(scale, scale, scale);

				T = XMMatrixTranslation(pos.x, pos.y, pos.z);

				T2 = XMMatrixTranslation(0, 0, 1);
				T3 = XMMatrixTranslation(0, 0, -1);

				Rx90 = XMMatrixRotationX(-XM_PI / 2);

				Ry = XMMatrixRotationY(-r);
				rot.y = -r;




				XMVECTOR det;

				return S * localRot * T2* Ry * T3 * T;
		}

	};


	

	XMFLOAT3 operator+(const XMFLOAT3 lhs, XMFLOAT3 rhs);
	XMFLOAT3 operator-(const XMFLOAT3 lhs, XMFLOAT3 rhs);
	XMFLOAT3 operator*(const XMMATRIX &lhs, XMFLOAT3 rhs);
	XMFLOAT3 operator*(XMFLOAT3 rhs, const XMMATRIX &lhs);
	XMFLOAT3 operator*(const XMFLOAT3 lhs, float rhs);
	XMFLOAT3 operator*(float rhs, const XMFLOAT3 lhs);
