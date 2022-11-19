#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

sf::Music gamemusic;

sf::SoundBuffer sbuffer;
sf::SoundBuffer hb;
sf::SoundBuffer hp;
sf::Sound newgame;
sf::Sound hitbrick;
sf::Sound hitpaddle;

sf::Font font;
sf::Text showScore;
sf::Text stostart;
sf::Text presssp;
sf::Text gameover;
int score = 0;

//Game States
#define SPLASH 0
#define MENU 1
#define GAME 2
#define END_GAME 3
int state = -1;

class CScore {
    public:
    std::vector<int> vhiscores;

    void Read()
    {
        std::ifstream in("hiscores.dat");
        if(in.good())
        {
            std::string str;
            getline(in,str);
            std::stringstream ss(str);
            int n;
            for(int i=0;i<5;i++)
            {
                ss >> n;
                vhiscores.push_back(n);
            }
            in.close();
        }
        else
        {
            //if file does not exist fill with 5 scores
            for(int i=0;i<5;i++)
            {
                vhiscores.push_back(0);
            }
        }
    }

    void Update(int newscore)
    {
        //new score to the end
        vhiscores.push_back(newscore);
        //sort
        sort(vhiscores.rbegin(), vhiscores.rend());
        //remove the last
        vhiscores.pop_back();
    }

    void Write()
    {
        std::ofstream out("hiscores.dat");
        for(int i=0;i<5;i++)
        {
            out << vhiscores[i] << " ";
        }
        out.close();
    }

    void Show(sf::RenderWindow &app)
    {
        sf::Text showHiScores;
        showHiScores.setFont(font);
        showHiScores.setCharacterSize(20);
        showHiScores.setPosition(120.f, 110.f);
        showHiScores.setFillColor(sf::Color::White);
        std::string histr="HiScores\n";
        for(int i=0;i<5;i++)
        {
            histr = histr + "    " + std::to_string(vhiscores[i]) + "\n";
        }
        showHiScores.setString(histr);
        app.draw(showHiScores);
    }
};

int main()
{
    srand(time(0));

    sf::RenderWindow app(sf::VideoMode(520, 450), "Arkanoid!");
    app.setFramerateLimit(60);

    sf::Texture t1,t2,t3,t4,t5,t6;
    t1.loadFromFile("images/block01.png");
    t2.loadFromFile("images/background.jpg");
    t3.loadFromFile("images/ball.png");
    t4.loadFromFile("images/paddle.png");
    t5.loadFromFile("images/splash.png");
    t6.loadFromFile("images/menu.png");

    sf::Sprite sBackground(t2), sBall(t3), sPaddle(t4);
    sf::Sprite sSplash(t5), sMenu(t6);
    sPaddle.setPosition(300,440);

    // load music
	if( !gamemusic.openFromFile("sound/John_Bartmann_-_08_-_Gameshow_Brazz.ogg"))
        return -1;
    gamemusic.setLoop(true);
    gamemusic.setVolume(50.f);

    // load sound in buffer
    if( !sbuffer.loadFromFile("sound/newgame.ogg")) return -1;
    if( !hb.loadFromFile("sound/ball-brick.wav")) return -1;
    if( !hp.loadFromFile("sound/ball-paddle.wav")) return -1;
    // prepare sound to play
    newgame.setBuffer(sbuffer);
    hitbrick.setBuffer(hb);
    hitpaddle.setBuffer(hp);

    // Load the text font
    font.loadFromFile("sansation.ttf");
    showScore.setFont(font);
    showScore.setCharacterSize(20);
    showScore.setPosition(240.f, 20.f);
    showScore.setFillColor(sf::Color::White);
    stostart.setFont(font);
    stostart.setCharacterSize(20);
    stostart.setPosition(100.f, 250.f);
    stostart.setFillColor(sf::Color::White);
    stostart.setString("Press S to start");
    gameover.setFont(font);
    gameover.setCharacterSize(50);
    gameover.setPosition(100.f, 250.f);
    gameover.setFillColor(sf::Color::White);
    gameover.setString("Game Over\nPress any key");
    presssp.setFont(font);
    presssp.setCharacterSize(20);
    presssp.setPosition(100.f, 250.f);
    presssp.setFillColor(sf::Color::White);
    presssp.setString("Press space");

    CScore cscore;
    cscore.Read();

    sf::Sprite block[100];

    //create 100 blocks.
    int n=0;
    for (int i=1;i<=10;i++)
    for (int j=1;j<=10;j++)
    {
         block[n].setTexture(t1);
         block[n].setPosition(i*43,j*20);
         n++;
    }

    float dx=6, dy=5;  //original values for speed
	float x=300, y=300; //ball position

	state = SPLASH;

    while (app.isOpen())
    {
        if( state == SPLASH )
        {
            sf::Event event;
            while (app.pollEvent(event))
            {
                if ((event.type == sf::Event::Closed) ||
                    ((event.type == sf::Event::KeyPressed)
                     && (event.key.code == sf::Keyboard::Escape)))
                    app.close();

                // Space key pressed: change state to MENU
                if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space))
                {
                    state = MENU;
                }
            }
        }

        if( state == MENU )
        {
            sf::Event event;
            while (app.pollEvent(event))
            {
                if ((event.type == sf::Event::Closed) ||
                    ((event.type == sf::Event::KeyPressed)
                     && (event.key.code == sf::Keyboard::Escape)))
                    app.close();

                // S key pressed: change state to GAME
                if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::S))
                {
                    state = GAME;
                    score = 0;
                    dx=3; dy=2; //slower
                    x=300; y=300;
                    //relocate the bricks. They are not deleted, only moved.
                    n=0;
                    for (int i=1;i<=10;i++)
                    for (int j=1;j<=10;j++)
                    {
                         block[n].setPosition(i*43,j*20);
                         n++;
                    }
                    newgame.play();
                    while( newgame.getStatus() == sf::SoundSource::Playing ){};
                    gamemusic.play();
                }
            }
        }

        if( state == GAME )
        {
            sf::Event event;
            while (app.pollEvent(event))
            {
                if ((event.type == sf::Event::Closed) ||
                    ((event.type == sf::Event::KeyPressed)
                     && (event.key.code == sf::Keyboard::Escape)))
                    app.close();
            }

            x+=dx;
            for (int i=0;i<n;i++)
                if ( sf::FloatRect(x+3,y+3,6,6).intersects(block[i].getGlobalBounds()) )
                     {
                         block[i].setPosition(-100,0); dx=-dx;
                         hitbrick.play();
                         score++;
                     }

            y+=dy;
            for (int i=0;i<n;i++)
                if ( sf::FloatRect(x+3,y+3,6,6).intersects(block[i].getGlobalBounds()) )
                     {
                         block[i].setPosition(-100,0); dy=-dy;
                         hitbrick.play();
                         score++;
                     }

            if (x<0 || x>520)  dx=-dx;
            if (y<0 /*|| y>450*/)  dy=-dy;
            if (y>450)
            {
                cscore.Update(score);
                state = END_GAME;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) sPaddle.move(6,0);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) sPaddle.move(-6,0);
            if ( sPaddle.getPosition().x < 0 ) sPaddle.setPosition(0,440);
            if ( sPaddle.getPosition().x > 430 ) sPaddle.setPosition(430,440);

            if ( sf::FloatRect(x,y,12,12).intersects(sPaddle.getGlobalBounds()) )
            {
                dy=-(rand()%5+2);
                hitpaddle.play();
            }

            sBall.setPosition(x,y);
        }

        if( state == END_GAME )
        {
            gamemusic.stop();
            sf::Event event;
            while (app.pollEvent(event))
            {
                if ((event.type == sf::Event::Closed) ||
                    ((event.type == sf::Event::KeyPressed)
                     && (event.key.code == sf::Keyboard::Escape)))
                    app.close();

                // Any key pressed: change state to MENU
                if (event.type == sf::Event::KeyPressed)
                {
                    state = MENU;
                }
            }
        }

        // game paint
        if( state == SPLASH )
        {
            app.draw(sSplash);
            app.draw(presssp);
        }

        if( state == MENU )
        {
            app.draw(sMenu);
            app.draw(stostart);
            cscore.Show(app);
        }

        if( state == GAME )
        {
            app.draw(sBackground);
            app.draw(sBall);
            app.draw(sPaddle);

            for (int i=0;i<n;i++)
             app.draw(block[i]);

            std::string sc = std::to_string(score);
            showScore.setString(sc);
            app.draw(showScore);
        }

        if( state == END_GAME )
        {
            app.draw(sBackground);
            app.draw(gameover);
        }

        app.display();
    }

    cscore.Write();

  return 0;
}
