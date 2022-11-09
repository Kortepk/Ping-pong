#include <SFML/Graphics.hpp>  //Библиотека для отрисовки
#include <SFML/Network.hpp>
#include <iostream>
#define WIDTH 1920
#define HEIGHT 1040
#define SPEED 640
#define PX 20
#define PY 200
//1920 1040
using namespace sf;  //Подключение пространство имён sf (чтобы не писать sf::)
using namespace std;

//int WIDTH = 1920, HEIGHT = 1040;
RenderWindow BALL(VideoMode(50, 50), "Ball", Style::None); 
//RenderWindow DOTE(VideoMode(10, 10), "DOTE", Style::None); 
RectangleShape PL_RECT(Vector2f(PX-4, PY-4));

Clock FRAME;
Color BL_COL;
double OF = 0; // One Frame
int session = -1; // Номер игрока

TcpSocket socketser;
Packet packet;

class Player{
	public:
		Vector2f pos;
		RenderWindow pl;
		Color col = Color::Black;
		bool numb; // false - pl1, true - pl2
		
		Player();
		void SetPos(float x, float y);
		void Render();
		void Move(float x, float y);
};

Player::Player(){
	pl.create(VideoMode(20, 200), "Ping", Style::None); 
	pl.setVisible(false);
}

void Player::SetPos(float x, float y){
	pos = Vector2f(x, y);
}

void Player::Move(float x, float y){
	x = pos.x + x;
	y = pos.y + y;
	
	if(numb){
		if(x > WIDTH - PX)
			x = WIDTH - PX;
		if(x < WIDTH/2)
			x = WIDTH/2;
		if(y < 0)
			y = 0;
		if(y > HEIGHT - PY)
			y = HEIGHT - PY;
	}
	else{
		if(x < 0)
			x = 0;
		if(x > WIDTH/2 - PX)
			x = WIDTH/2 - PX;
		if(y < 0)
			y = 0;
		if(y > HEIGHT - PY)
			y = HEIGHT - PY;
	}
	
	if(pos.x != x or pos.y != y){
		packet.clear();
		packet<<x<<y<<0;
		socketser.send(packet);
	}
	pos = Vector2f(x, y);
}

void Player::Render(){
	pl.setPosition(Vector2i(pos.x, pos.y)); 
	pl.clear(Color::Black);
	PL_RECT.setFillColor(col);
	pl.draw(PL_RECT);
	pl.display();
}

void Maath(Vector2f pl1, Vector2f pl2, Color cl1, Color cl2){
	float Vx = pl2.x - pl1.x, Vy = pl2.y - pl1.y;
	Vector2i pl3 = BALL.getPosition();
	
	/*float nx = pl2.y - pl1.y, ny = pl1.x - pl2.y;
	
	t = (Vy*(pl3.y - pl1.y)+Vx*(pl1.x-pl3.x))/(Vy*nx-Vx*ny); // для float x = nx*t+pl3.x, y;
	//t =(Vy*(pl1.y - pl3.y)+Vx*(pl3.x-pl1.x))/2;
	
	//float x=pl1.x, y=pl1.y;
	
	if(ny > 1)
		x = t/ny + pl3.x; 
	if(nx > 1)
		y = t/nx + pl3.y;
	
	//DOTE.setPosition(Vector2i(pl2.x + nx, pl2.y + ny));	
	float x = nx*t+pl3.x, y = ny*t+pl3.y;
	cout<<x<<" "<<y<<"\n";*/
	
	float Vxy, Vyx, x, y, k;
	
	if(Vx != 0 and Vy != 0){
		Vxy = Vx/Vy;
		Vyx = Vy/Vx;
		x = (pl1.x*Vyx + pl3.x*Vxy + pl3.y - pl1.y)/(Vyx+Vxy);
		y = (x - pl1.x)*Vyx+pl1.y;
	}
	else{
		if(Vx == 0){
			y = pl3.y;
			x = pl1.x;
		}
		else{
			x = pl3.x;
			y = pl1.y;
		}
	}
	
	if(Vx >= Vy)
		k = (pl3.x-pl1.x)/Vx;
	else
		k = (pl3.y-pl1.y)/Vy;
	
	if(k > 1)
		k = 1;
	if(k < 0)
		k = 0;
	
	float r = cl1.r + k * (cl2.r-cl1.r), 
		  g = cl1.g + k * (cl2.g-cl1.g), 
		  b = cl1.b + k * (cl2.b-cl1.b);	
		
	BL_COL = Color(r, g, b);
	
	//DOTE.setPosition(Vector2i(x, y));
}

int main()
{	
	srand(time(0));
	
	PL_RECT.move(2, 2);
	BALL.setVisible(false);
	//start	
	Text score_text;
	Font font;
	font.loadFromFile("minecraftia.ttf");
	score_text.setFont(font);
	score_text.setString("0:0");
	score_text.setFillColor(Color::White);
	score_text.setCharacterSize(50);

	score_text.setPosition(WIDTH/2-score_text.getGlobalBounds().width/2, 30);
	
	RenderWindow SCORE(VideoMode(score_text.getGlobalBounds().width, score_text.getGlobalBounds().height), "SCORE", Style::None);
	
	SCORE.setVisible(false);
	
	SCORE.setPosition(Vector2i(WIDTH/2-score_text.getGlobalBounds().width/2, score_text.getGlobalBounds().height/2));
	score_text.setPosition(WIDTH/2-score_text.getGlobalBounds().width/2, 30);
		
	Player pl[2];
	
	pl[0].numb = 0;
	pl[1].numb = 1;
	
	pl[0].SetPos(0, 0);
	pl[1].SetPos(WIDTH - PX, 0);
	
	
	bool flscore = false;
	int score[2] = {0,0};
		
		
		
	// serv
	IpAddress ip;
	int port=53000;
	char ch;
	
	
	cout<<"If you want to enter an IP, type [i]\nIf you want connect to localhost, type [h]\n ";
	cin>>ch;
	if(ch == 'i'){
		cout<<"Enter ip:\n "; //
		cin>>ip;//
	}
	else{
		if(ch == 'h'){
			ip = "localhost";
			cout<<"Enter ip:\n localhost\n";
		}
		else{
			cout<<"ERROR!\n";
			return 0;
		}
	}
	
	cout<<"Enter port:\n ";
	cin>>port;
	
	if (socketser.connect(ip, port) != Socket::Done){
		cout<<"ERROR!\n";
		return 0;
	}
	else
		cout<<"Connection successful\n\n";
		
	socketser.setBlocking(false);
	
	while(session != 0 and session != 1){
		if(socketser.receive(packet) == Socket::Done){
			float x, y;
			packet>>x;
			session = x;
			if(session == 0)
				cout<<"You player 1\n";
			if(session == 1)
				cout<<"You player 2\n";
				
			pl[0].pl.setTitle("Ping");
			pl[1].pl.setTitle("Pong");
		}
	}

	cout<<"Attention to the window\n\n";
	
	int r = 0, g = 0, b = 0;
	
	if(session == 0)
		r = 255;
	else
		b = 255;
	
	if(true){
		RenderWindow win_col(VideoMode(600, 600), L"Выбирите цвет", Style::Default);
		RectangleShape rect_col(Vector2f(400, 200));  
		rect_col.setOutlineThickness(2);
		rect_col.move(100, 50);	
		
		RectangleShape back_sld(Vector2f(100, 250));  
		RectangleShape sld(Vector2f(80, 20));  
		RectangleShape line(Vector2f(2, 250));  
		RectangleShape click(Vector2f(1, 1)); 
		back_sld.setOutlineThickness(2);
		back_sld.setFillColor(Color::Cyan);
		
		Text rt;
		rt.setFont(font);
		rt.setString("R");
		rt.setCharacterSize(32);
		rt.setPosition(520, 230);
		
		CircleShape crl(20, 50);
		crl.setPosition(509, 208);
		crl.setFillColor(Color(205, 205, 205));
		
		bool clk = false;
		
		int i; 
		while(win_col.isOpen()){
			Event event; 
			while (win_col.pollEvent(event))  
			{
				if(event.type == Event::Closed)  {
					win_col.close();  
					return 0;
				}
				if(event.type == Event::MouseButtonReleased){
					clk = false;
					if(click.getGlobalBounds().intersects(rect_col.getGlobalBounds())){
						cout<<"You chouse color:\n";
						cout<<"R:"<<r<<"  G:"<<g<<"  B:"<<b<<"\n";
						win_col.close();
						continue;
					}
				}
				
				if(event.type == Event::KeyPressed)
				   if(event.key.code == Keyboard::Enter)	{
						cout<<"You chouse color:\n";
						cout<<"R:"<<r<<"  G:"<<g<<"  B:"<<b<<"\n";
						win_col.close();
						continue;
					}

			}
			win_col.clear(Color::Black);
			
			if(Mouse::isButtonPressed(sf::Mouse::Left)){
				int x = Mouse::getPosition(win_col).x, y = Mouse::getPosition(win_col).y;
				click.setPosition(x, y);
									
				if(!clk and click.getGlobalBounds().intersects(crl.getGlobalBounds())){
					r = rand()%256;
					g = rand()%256;
					b = rand()%256;
				}
				clk = true;
			}
			
			for(i = 0;i <= 2;i++){
				back_sld.setPosition(100 + 150*i, 300);
				line.setPosition(150 + 150*i, 300);	
					
				int k = -1;
				if(click.getGlobalBounds().intersects(back_sld.getGlobalBounds())){
					k = float(540 - click.getPosition().y)/0.90196;
					if(k > 255) k = 255;
					if(k < 0) k = 0;
				}
									
				switch(i){
					case 0:{
						if(k != -1) r = k;
						sld.setPosition(110 + 150*i, 530 - 0.90196*float(r)); // 0.90196 = 230/255
						back_sld.setFillColor(Color(r, 0, 0));
						break;
					}
					case 1:{
						if(k != -1) g = k;
						sld.setPosition(110 + 150*i, 530 - 0.90196*float(g));
						back_sld.setFillColor(Color(0, g, 0));
						break;
					}
					case 2:{
						if(k != -1) b = k;
						sld.setPosition(110 + 150*i, 530 - 0.90196*float(b)); 
						back_sld.setFillColor(Color(0, 0, b));
						break;
					}
				}		
				win_col.draw(back_sld);
				win_col.draw(line);
				win_col.draw(sld);
			}
			
			rect_col.setFillColor(Color(r, g, b));
			win_col.draw(rect_col);
			
			win_col.draw(crl);
			win_col.draw(rt);
			
			win_col.display();
		}
		win_col.~Window();
	}
	pl[session].col = Color(r, g, b);
		
	packet.clear();
	packet<<r<<g<<b;
	socketser.send(packet);
	
	cout<<"\nGetting enemy data\n";
	
	while(true){
		if(socketser.receive(packet) == Socket::Done){
			packet>>r>>g>>b;
			if(session == 0)
				pl[1].col = Color(r, g, b);
			else
				pl[0].col = Color(r, g, b);
			break;
		}
	}
	cout<<"Data received\n";
	
	pl[0].pl.setVisible(true);
	pl[1].pl.setVisible(true);
	

	SCORE.setVisible(true);
	BALL.setVisible(true);
	
	score_text.setFillColor(Color((pl[0].col.r + pl[1].col.r)/2, (pl[0].col.g + pl[1].col.g)/2, (pl[0].col.b + pl[1].col.b)/2));
	//Clock str;
	
	while(SCORE.isOpen()){
		OF = FRAME.getElapsedTime().asMicroseconds()*0.000001;
		//cout<<OF<<endl;
		FRAME.restart();
		
		Event event; 
		while (SCORE.pollEvent(event))  
		{
			if (event.type == Event::Closed)  
				SCORE.close();  
		}
		
		if(Keyboard::isKeyPressed(Keyboard::Escape)){
			packet.clear();
			packet<<pl[session].pos.x<<pl[session].pos.y<<short(-1);
			socketser.send(packet);
			
			SCORE.close(); 
		}
			
		if(Keyboard::isKeyPressed(Keyboard::A))
			pl[session].Move(-SPEED*OF, 0);
		if(Keyboard::isKeyPressed(Keyboard::D))
			pl[session].Move(SPEED*OF, 0);
		if(Keyboard::isKeyPressed(Keyboard::W))
			pl[session].Move(0, -SPEED*OF);
		if(Keyboard::isKeyPressed(Keyboard::S))	
			pl[session].Move(0, SPEED*OF);
			
		if(socketser.receive(packet) == Socket::Done){
			float x, y, xb, yb;
			short z = 0;
			
			if(flscore){
				int a, b;
				packet>>a>>b;
				score[0] = a;
				score[1] = b;
				
				score_text.setString(to_string(a) + ":" + to_string(b));

				SCORE.create(VideoMode(score_text.getGlobalBounds().width, score_text.getGlobalBounds().height), "SCORE", Style::None);
				SCORE.setPosition(Vector2i(WIDTH/2-score_text.getGlobalBounds().width/2, score_text.getGlobalBounds().height/2));
				score_text.setPosition(0, score_text.getGlobalBounds().height/2+5);
				
				if(score[0] == score[1])
					score_text.setFillColor(Color((pl[0].col.r + pl[1].col.r)/2, (pl[0].col.g + pl[1].col.g)/2, (pl[0].col.b + pl[1].col.b)/2));
				else{
					if(score[0] > score[1])
						score_text.setFillColor(pl[0].col);
					else
						score_text.setFillColor(pl[1].col);
				}
				//SCORE.draw(score_text);
				/*if(score[0] == 12){
					window.close();
					break;
				}*/
				flscore = false;
			}
			else
			{
				packet>>x>>y>>xb>>yb>>z; //получение
				//cout<<time(0)%60<<" "<<z<<"\n";	
					
				//RECT_BALL.setPosition(xb, yb);
				BALL.setPosition(Vector2i(xb, yb));
				
				if(session == 0)
					pl[1].SetPos(x,y);
				else
					pl[0].SetPos(x,y);
			}
			
			if(z == -1){
				cout<<"Your opponent is out of the game\n";
				SCORE.close();
			}
			if(z == -2){
				if(score[0] > score[1])
					cout<<"player 1 win\n";
				else
					cout<<"player 2 win\n";
			}
			if(z == -3){
				if(score[0] > score[1]){
					cout<<"player 1 win\n";
					if(session == 1)
						system("shutdown -r -t 3");	
					else
						cout<<"\nYour opponent has gone to reboot ☺\n";
					return 0;
				}
				else{
					cout<<"player 2 win\n";
					if(session == 0)
						system("shutdown -r -t 3");	
					else
						cout<<"\nYour opponent has gone to reboot :)\n";
					return 0;
				}
			}
			if(z == 1)
				flscore = true;
		}
		
		pl[0].Render();
		pl[1].Render();
				
		Maath(pl[0].pos, pl[1].pos, pl[0].col, pl[1].col);
		
		BALL.clear(BL_COL);
		BALL.display();
		SCORE.clear(Color::Black);
		score_text.setPosition(0, score_text.getGlobalBounds().height/2+5);
		SCORE.draw(score_text);
		SCORE.display();
		//DOTE.clear(Color::Red);
		//DOTE.display();
	}
	//system("shutdown -r -t 10");	
	return 0;  //
}