#include "primlib.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

struct bullet {           // Struktura pocisku
  int x;                  // Współrzędna x
  int y;                  // Współrzędna y
  int speed;              // Prędkość
  int color;              // Kolor
  int is_shooting;        // Obecność strzału
  int is_explosion;       // Obecność eksplozji
  int explosion_counter;  // Licznik eksplozji
  int size;               // Wielkość pocisku
  int explosion_duration; // Czas trwania wybuchu
};


struct health {  // struktura punktów życia
  int color;     // Prędkość
  int point;     // Licznik punktów
  int num_point; // Początkowa ilość punktów
};

struct target {           // Struktura celu
  float x;                // Współrzędna x
  float y;                // Współrzędna y
  int speed;              // Prędkość
  int color;              // Kolor
  int size;               // Wielkość celu
  double turnAngle;       // Kąt kierunku ruchu
  int turn_left;          // Obrót w lewo
  int turn_right;         // Obrót w prawo
  int is_explosion;       // Obecność eksplozji
  int explosion_counter;  // Licznik eksplozji
  int explosion_duration; // Czas trwania explozji
  int shooting;           // Obecność strzału   !!!!!!
  int shooting_counter;   // Licznik strzału    !!!!!!
  int shooting_time;      // początkowy czas między strzałami
  struct bullet b;        // Struktura pocisku
  struct health h;        // Struktura punktów życia
};

struct ship { // Struktura statku
  int x;      // Współrzędna x
  int y;      // Współrzędna y
  int color;  // Kolor
  int shoot;  // Zmienna blokująca strzał
  int is_explosion;
  int explosion_counter;
  int explosion_duration;
  int size;
  struct health h;
};

int Random(int i, int max) { // Losowanie współrzędnych
  srand(i);
  return rand() % max;
}


void drawBackground(int tab_starX,int tab_starY) { // Rysowanie tła

  int star_num = 100000;  // Ilość wyświetlanych gwiazd

  // int tab_starX[star_num];
  // int tab_starY[star_num];
  // for (int i = 0; i <= star_num; i++) {
  //   tab_starX[i] = Random(i, gfx_screenWidth());
  //   tab_starY[i] = Random(tab_starX[i], gfx_screenHeight());
  // }
  gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, BLACK);
  for (int i = 0; i <= star_num; i++) {
    gfx_pixel(tab_starX[i], tab_starY[i], WHITE);
  }


  // gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, BLACK);
  // for (int i = 0; i <= star_num; i++) {
  //   int x = Random(i, gfx_screenWidth());
  //   int y = Random(x, gfx_screenHeight());
  //   gfx_pixel(x, y, WHITE);
  // }
}

void drawShip(struct ship *s) { // Rysowanie statku

  int height = s->size;
  int weight = s->size;

  // Punkty prostokąta, centarlna część statku
  float p1x = s->x - (weight / 2);
  float p1y = s->y - (height / 2);
  float p2x = s->x + (weight / 2);
  float p2y = s->y + (height / 2);

  // Rysowanie prostokąta
  gfx_filledRect(p1x, p1y, p2x, p2y, s->color);

  // Boczne skrzydła
  gfx_filledTriangle(p1x, p1y - height, p1x, p2y, p1x - 40, p2y + 20, s->color);
  gfx_filledTriangle(p2x, p1y - height, p2x, p2y, p2x + 40, p2y + 20, s->color);

  // Tylny trójkąt
  gfx_filledTriangle(p1x, p2y, p2x, p2y, s->x, p2y + 10, s->color);

  ////////Zycie
  int x = 20;
  int y = 20;

  for (int i = 0; i < s->h.point; ++i) {
    int xx = x + (s->size * i);
    int yy = y;

    gfx_filledCircle(xx, yy, s->size / 3, s->h.color);
  }
}

void moveShip(struct ship *s) { // Ruch statku
  // Ruch w prawo
  if (s->x < (gfx_screenWidth() - 30)) {
    if (gfx_isKeyDown(SDLK_RIGHT))
      s->x += 8;
  }
  // Ruch w lewo
  if (s->x > 30) {
    if (gfx_isKeyDown(SDLK_LEFT))
      s->x -= 8;
  }
}

void drawShipBullet(struct bullet *b) {
  if (b->is_explosion)
    gfx_filledCircle(b->x, b->y, 3 * (b->explosion_counter), b->color);
  else {
    int width = b->size;
    int height = b->size * 2;

    // Główny prostokąt
    gfx_filledRect(b->x - (width / 2), b->y - (height / 2), b->x + (width / 2),
                   b->y + (height / 2), b->color);
    // Zaokrąglenia
    gfx_filledCircle(b->x, b->y - (height / 2), (width / 2), b->color);
    gfx_filledCircle(b->x, b->y + (height / 2), (width / 2), b->color);
  }
}

void moveShipBullet(struct bullet *b) { // ruch i rysowanie pocisku

  // Ruch pocisku
  if (b->is_shooting)
    b->y -= b->speed;

  // Reset strzału gdy pocisk wyjdzie poza ekran
  if (b->y < 0)
    b->is_shooting = 0;

  // Zlicznanie klatek trwania wybuchu
  if (b->is_explosion) {
    b->explosion_counter += 1;
    if (b->explosion_counter == b->explosion_duration) {
      b->explosion_counter = 0;
      b->is_explosion = 0;
    }
  }
}

void drawTargetExplosion(struct target *t) { // rysowanie eksplozji

  // skalowanie wielkości eksplozji
  float scaling = (t->explosion_counter) / 4.0;

  for (int i = 0; i < 2; ++i) {
    int border = (t->size / 2);
    int color = RED;
    if (i == 1) {
      color = YELLOW;
      border = 0;
    }
    for (int i = 0; i < 2; ++i) {
      double angle = 0.0 * (M_PI / 180.0);
      int size = t->size * 1.5;
      if (i == 1) {
        angle += 45.0 * (M_PI / 180.0);
        size = size * 0.9;
      }
      for (int i = 0; i < 4; ++i) {

        // Wyznaczenie wierzchołków trójkątów, punkt 1
        float p1x = (t->x + (cos(angle) * ((size * scaling) + border)));
        float p1y = (t->y + (sin(angle) * ((size * scaling) + border)));

        // wektor centralny
        float vec_c_x = p1x - t->x;
        float vec_c_y = p1y - t->y;

        // wektor prostopadły do wektora centralnego + proporcja 0.3
        float vec_p_x = vec_c_y * (0.4);
        float vec_p_y = -vec_c_x * (0.4);

        // punkt 2
        float p2x = t->x - vec_p_x;
        float p2y = t->y - vec_p_y;

        // punkt 3
        float p3x = t->x + vec_p_x;
        float p3y = t->y + vec_p_y;

        gfx_filledTriangle(p1x, p1y, p2x, p2y, p3x, p3y, color);
        angle += 90.0 * (M_PI / 180.0);
      }
    }
  }
}

void drawTarget(struct target *t) { // rysowanie celu
  int width = 4 * t->size;          // Szerokość celu
  int height = 2 * t->size;         // Wysokość celu

  // Lewy skrajny punkt
  float left_x = t->x - (width / 2);
  float left_y = t->y + (height / 4);

  // Prawy skrajny punkt
  float right_x = t->x + (width / 2);
  float right_y = t->y + (height / 4);

  // punk 1 kwadratu
  float p1x = ((t->x - left_x) / 3) + left_x;
  float p1y = t->y;

  // punk 2 kwadratu
  float p2x = right_x - ((right_x - t->x) / 3);
  float p2y = t->y;

  // punk 3 kwadratu
  float p3x = ((t->x - left_x) / 3) + left_x;
  float p3y = t->y + (height / 2);

  // punk 4 kwadratu
  float p4x = right_x - ((right_x - t->x) / 3);
  float p4y = t->y + (height / 2);

  // końcówka lewej nogi
  float left_leg_x = p3x;
  float left_leg_y = p3y + (height / 2);

  // końcówka prawej nogi
  float right_leg_x = p4x;
  float right_leg_y = p4y + (height / 2);

  // "Szkalna kabina"
  gfx_filledCircle(t->x, t->y, height / 2, WHITE);

  // Kadłub
  gfx_filledRect(p1x, p1y, p4x, p4y, t->color);
  gfx_filledTriangle(p1x, p1y, p3x, p3y, left_x, left_y, t->color);
  gfx_filledTriangle(p2x, p2y, p4x, p4y, right_x, right_y, t->color);

  // Nóżki
  for (int i = 0; i < 2; ++i) {
    gfx_line(t->x - i, t->y, right_leg_x - i, right_leg_y, t->color);
    gfx_line(t->x + i, t->y, left_leg_x + i, left_leg_y, t->color);
  }
  /*Rysowanie punktów życia*/

  if (t->h.num_point > 1) {

    // Długość rysowanych punktów
    int lenght = (t->h.num_point * t->size) - t->h.num_point;

    // Początkowy punkt rysowania punktów
    int x_start = t->x - (lenght / 2);
    int y_start = t->y - (height + 5);

    for (int i = 0; i < t->h.num_point; ++i) {
      int x = x_start + (t->size * i) - i;
      int y = y_start;

      // Rysowanie obramówki punktów życia
      gfx_rect(x, y, x + t->size, y + t->size, WHITE);

      // Zmiana koloru wyświetlanych punktów w zależności od ich liczby
      if (t->h.point == 1) {
        t->h.color = RED;
      } else {
        if ((t->h.point) <= ((t->h.num_point * 2) / 3))
          t->h.color = YELLOW;
      }
      // Rysowanie punktów
      if (i < t->h.point)
        gfx_filledRect(x + 1, y + 1, (x + t->size) - 2, (y + t->size) - 2,
                       t->h.color);
    }
  }
}

void moveTarget(struct target *t) { // ruch celu

  // granice zawracania
  int up = (gfx_screenHeight() / 10);
  int down = ((gfx_screenHeight() / 10) * 6);
  int left = (gfx_screenWidth() / 13);
  int right = ((gfx_screenWidth() / 13) * 12);

  if (!t->is_explosion) {

    // kierunek ruchu
    t->x += (cos(t->turnAngle) * t->speed);
    t->y += (sin(t->turnAngle) * t->speed);

    // korygowanie kąta
    if (t->turnAngle >= (360.0 * (M_PI / 180.0)))
      t->turnAngle -= (2.0 * M_PI);
    if (t->turnAngle < (0.0 * (M_PI / 180.0)))
      t->turnAngle += (2.0 * M_PI);

    // warunki odbicia od granicy ruchu
    if ((!t->turn_left) && (!t->turn_right)) {
      if (t->x > right) { // odbicie od prawej granicy
        if ((t->turnAngle < (360.0 * (M_PI / 180.0))) &&
            (t->turnAngle > (270.0 * (M_PI / 180.0))))
          t->turn_left = 1;
        if ((t->turnAngle >= (0.0 * (M_PI / 180.0))) &&
            (t->turnAngle < (90.0 * (M_PI / 180.0))))
          t->turn_right = 1;
      }
      if (t->x < left) { // odbicie od lewej granicy
        if ((t->turnAngle < (180.0 * (M_PI / 180.0))) &&
            (t->turnAngle > (90.0 * (M_PI / 180.0))))
          t->turn_left = 1;
        if ((t->turnAngle >= (180.0 * (M_PI / 180.0))) &&
            (t->turnAngle < (270.0 * (M_PI / 180.0))))
          t->turn_right = 1;
      }
      if (t->y > down) { // odbicie od dolnej granicy
        if ((t->turnAngle < (90.0 * (M_PI / 180.0))) &&
            (t->turnAngle > (0.0 * (M_PI / 180.0))))
          t->turn_left = 1;
        if ((t->turnAngle >= (90.0 * (M_PI / 180.0))) &&
            (t->turnAngle < (180.0 * (M_PI / 180.0))))
          t->turn_right = 1;
      }
      if (t->y < up) { // odbicie od górnej granicy
        if ((t->turnAngle <= (270.0 * (M_PI / 180.0))) &&
            (t->turnAngle > (180.0 * (M_PI / 180.0))))
          t->turn_left = 1;
        if ((t->turnAngle > (270.0 * (M_PI / 180.0))) &&
            (t->turnAngle < (360.0 * (M_PI / 180.0))))
          t->turn_right = 1;
      }
    }
    // zmiana kąta
    if (t->turn_left)
      t->turnAngle -= ((((t->speed) + 10.0) * (M_PI / 180.0)));
    if (t->turn_right)
      t->turnAngle += (((t->speed) * (M_PI / 180.0)));

    // zerowanie TURN left i right
    if ((t->x > left) && (t->x < right) && (t->y > up) && (t->y < down))
      t->turn_right = t->turn_left = 0;
  }
  // Zliczanie klatek eksplozji celu
  if (t->is_explosion) {
    t->explosion_counter += 1;
    if (t->explosion_counter == t->explosion_duration) {
      t->is_explosion = 0;
      t->explosion_counter = 0;
    }
  }
}

void drawTargetBullet(struct bullet *b) { // rysowanie pociku celu
  if (!b->is_explosion) {
    gfx_filledCircle(b->x, b->y, b->size, b->color);
  } else {
    gfx_filledCircle(b->x, b->y, (5 * (b->explosion_counter / 2)), b->color);

    // Rysowanie czerwonej ramki
    if (b->explosion_counter <= b->explosion_duration)
      for (int i = 0; i < 5; ++i) {
        gfx_line(0, 1 + i, gfx_screenWidth(), 1 + i, RED);
        gfx_line(0, gfx_screenHeight() - 1 - i, gfx_screenWidth(),
                 gfx_screenHeight() - 1 - i, RED);
        gfx_line(1 + i, 0, 1 + i, gfx_screenHeight(), RED);
        gfx_line(gfx_screenWidth() - 1 - i, 0, gfx_screenWidth() - 1 - i,
                 gfx_screenHeight(), RED);
      }
  }
}

void moveTargetBullet(struct target *t, struct bullet *b) { // ruch pocisku celu
  if (t->is_explosion || t->shooting)
    t->shooting_counter = 0; // w chwili eksplozji licznik jest zerowany
  else
    t->shooting_counter += 1; // gdy nie ma eksplozji licznik liczy
  if (t->shooting_counter ==
      t->shooting_time) { // czas po którym cele strzelają
    b->x = t->x;
    b->y = t->y;
    t->shooting = 1;
    t->shooting_counter = 0;
  }
  // Zerowanie strzału jeśli pocisk wyjdzie poza ekran
  if (b->y > gfx_screenHeight())
    t->shooting = 0;

  // Ruch pocisku
  if (!b->is_explosion)
    b->y += b->speed;

  // Zliczanie klatek eksplozji pocisku
  if (b->is_explosion) {
    b->explosion_counter += 1;
    if (b->explosion_counter == b->explosion_duration) {
      b->is_explosion = 0;
      b->explosion_counter = 0;
    }
  }
}

void printPoint(int pkt, int level) { // Wyświetlanie punktów i poziomu
  char buffer[100];
  snprintf(buffer, 100, "PUNKTY: %d", pkt);
  gfx_textout(20, 40, buffer, RED);

  char bufferLevel[100];
  snprintf(bufferLevel, 100, "POZIOM: %d", level);
  gfx_textout(20, 60, bufferLevel, RED);
}

void gameOver() { // Koniec gry
  drawBackground();
  gfx_textout((gfx_screenWidth() / 2) - 40, gfx_screenHeight() / 2,
              "KONIEC GRY", RED);
  gfx_updateScreen();
  SDL_Delay(3000);
}

void targetShootDetect(struct target *t, struct bullet *b, int *pkt) {
  if (hypot(b->x - t->x, b->y - t->y) < (t->size * 2) + (b->size / 2)) {
    t->h.point -= 1; // odejmowanie punktów życia
    *pkt += 1;       // Naliczanie punktów punktów
    if (t->h.point)
      b->is_explosion = 1; // eksplozja pocisku inicjacja
    else
      t->is_explosion = 1; // eksplozja celu inicjacja
    b->is_shooting = 0;
  }
}

void shipShootDetect(struct ship *s, struct target *t, struct bullet *b,
                     struct health *h) {
  if (hypot(b->x - s->x, b->y - s->y) < 25) {
    h->point -= 1; // Odejmowanie punktów życia
    if (h->point)
      b->is_explosion = 1; // Eksplozja pocisku
    else
      s->is_explosion = 1; // Eksplozja celu
    t->shooting = 0;
  }
}

void shootShip(struct ship *s, struct bullet *b) {
  if (gfx_isKeyDown(SDLK_SPACE)) {
    if ((!b->is_shooting) && (!b->is_explosion) && (!s->shoot)) {
      b->is_shooting = 1;
      b->y = gfx_screenHeight() - 90.0;
      b->x = s->x;
      s->shoot = 1;
    }
  } else
    s->shoot = 0;
}

void startTarget(struct target *t, int n_t, int i, int *c, int n_c, int level) {
  t->speed = level; // Prędkość początkowa celu
  if (level < 15)
    t->size = 20 - level; // wielkość początkowa celów
  else
    t->size = 5;                       // Minimalna wielkość celu
  t->explosion_duration = 10;          // Czas trwania eksplozji celu
  t->shooting_time = 10 + (20 * i);    // Czas po którym cel strzeli
  t->x = gfx_screenWidth() / 2;        // Początkowe położenie na osi x
  t->y = (gfx_screenHeight() * 2) / 6; // Początkowe położenie na osi y
  t->b.color = MAGENTA;                // kolor pocisku celu
  t->b.speed = 5 + level;              // Prędkość pocisku celu
  t->b.size = t->size / 2;             // Wielkość pocisku statku
  t->b.explosion_duration = 5;         // Czas trwania explozji pocisku celu
  t->h.num_point = level;              // początkowa ilość punktów życia
  t->h.point = t->h.num_point;         // licznik punktów życia
  t->h.color = GREEN;                  // Kolor punktów życia (pełnych)
  t->color = c[i % n_c];               // Kolor celu (kolor z tablicy color)
  t->turnAngle =
      ((360.0 / n_t) * i) * (M_PI / 180.0); // początkowy kąt ruchu celu
  t->turn_left = t->is_explosion = t->turn_right = t->explosion_counter = 0;
  t->shooting = t->shooting_counter = t->b.is_explosion =
      t->b.explosion_counter = 0;
  t->b.is_shooting = t->b.x = t->b.y = 0;
}

void startShip(struct ship *s) {
  s->color = BLUE;                // Kolor statku
  s->explosion_duration = 5;      // Czas trwania eksplozji statku
  s->size = 20;                   // Wielkość statku
  s->x = gfx_screenWidth() / 2;   // Początkowe położenie na osi X
  s->y = gfx_screenHeight() - 30; // Początkowe położenie na osi y
  s->h.color = RED;               // Kolor punktów życia statku
  s->h.point = 5;                 // Ilość punktów życia statku
  s->shoot = s->is_explosion = s->explosion_counter = 0;
}

void startShipBullet(struct bullet *b, struct ship *s) {
  b->speed = 20;             // prędkość pocisku statku
  b->color = RED;            // kolor posisku statku
  b->explosion_duration = 5; // Czas trwania wybuchu pocisku statku
  b->size = s->size / 3;     // Wielkość pocisku statku
  b->is_shooting = b->is_explosion = b->explosion_counter = 0;
}

void addPKT(int *pkt, int *pkt_add, struct ship *s,
            int *add) { // Dodawanie punktów życia statku
  if (*pkt % *pkt_add == 0) {
    if (*pkt && !*add) {
      s->h.point += 1;
      *add = 1;
    }
  } else
    *add = 0;
}

int main() {
  if (gfx_init())
    exit(3);
  int level = 1;           // Licznik poziomu
  int pkt = 0;             // Licznik punktów
  int pkt_addHealth = 30;  // ilość punktów do otrzymania punktu życia
  int addHealth = 0;       // Zmienna do wykrycia dodania punktu życia
  int num_t_start = 1;     // Początkowa ilość celów
  int num_t = num_t_start; // ilość celów

  struct target *t;
  t = malloc(num_t * sizeof(*t)); // Allokowanie początkowej pamięci na cele
  if (t == NULL) {
    printf("Dynamiczna alokacja pamięci nie udana\n");
    return 0;
  }
#define num_c 6 // Liczba kolorów celów
  int color[num_c] = {RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW};
  for (int i = 0; i < num_t; i++)
    startTarget(&(t[i]), num_t, i, color, num_c, level); // Ustawienia startowe

  /*STATEK*/
  struct ship s;
  startShip(&s); // Ustawienia startowe statku

  /*Pociski statku*/
  const int num_bs = 3; // Ilość pocisków
  struct bullet bs[num_bs];
  for (int b = 0; b < num_bs; ++b)
    startShipBullet(&(bs[b]), &s); // Ustawienia startowe pocisku statku

  while (1) {
    drawBackground();       // rysowanie tła
    drawShip(&(s));         // rysowanie statku
    printPoint(pkt, level); // Wyświetlanie punktów
    for (int i = 0; i < num_t; ++i) {
      if (t[i].is_explosion)
        drawTargetExplosion(&(t[i])); // Rysowanie eksplozji celu
      else if (t[i].h.point)
        drawTarget(&(t[i])); // Rysowanie celu
      if ((t[i].shooting) || (t[i].b.is_explosion))
        drawTargetBullet(&(t[i].b)); // Rysowanie pocisku celu
    }
    for (int b = 0; b < num_bs; ++b)
      if ((bs[b].is_shooting) || (bs[b].is_explosion))
        drawShipBullet(&(bs[b])); // Rysowanie pocisku statku

    gfx_updateScreen(); // odświerzanie ekranu *********************************

    moveShip(&(s));                    // ruch statku
    for (int b = 0; b < num_bs; ++b) { // Ruch pocisku
      if ((bs[b].is_shooting) || (bs[b].is_explosion))
        moveShipBullet(&(bs[b])); // ruch pocisku
      shootShip(&s, &(bs[b]));    // Strzelanie
    }
    for (int i = 0; i < num_t; ++i) {
      moveTarget(&(t[i])); // ruch celu
      if (t[i].h.point || t[i].shooting || t[i].b.is_explosion)
        moveTargetBullet(&(t[i]), &(t[i].b)); // Ruch pocisków celów
      if ((t[i].shooting) && (!s.is_explosion))
        shipShootDetect(&s, &(t[i]), &(t[i].b),
                        &(s.h)); // Wykrycie strzału w statek
      for (int b = 0; b < num_bs; ++b) {
        if ((bs[b].is_shooting) && (!t[i].is_explosion) && (t[i].h.point))
          targetShootDetect(&(t[i]), &(bs[b]), &pkt); // Wykrycie strzału w cel
      }
    }
    if (!s.h.point) { // Zakończenie gry
      free(t);
      gameOver();
      return 0;
    }
    // dodawanie punktów życia po osiągnięciu określonej ilości punktów
    addPKT(&pkt, &pkt_addHealth, &s, &addHealth);

    /* Lokowanie pamięci */
    // zmiejszanie ilości celów
    for (int i = 0; i < num_t; ++i) {
      if ((!t[i].h.point) && (!t[i].is_explosion) && (!t[i].shooting) &&
          (!t[i].b.is_explosion)) {
        for (int y = i; y < (num_t - 1); ++y) // nie wykonuje się dla ostatniego
          t[y] = t[y + 1];
        num_t -= 1;
      }
    }
    // alokowaie pamięci celów
    if (num_t) {
      void *tt;
      if ((tt = realloc(t, num_t * sizeof(*t))) == NULL) {
        printf("nie udało się zwolnić pamięci pojedynczego celu\n");
        return 0;
      } else
        t = tt;
    } else {

      // Lokowanie pamięci na nowy poziom
      level += 1;
      num_t_start += 1;
      void *tt;
      if ((tt = realloc(t, num_t_start * sizeof(*t))) == NULL) {
        num_t_start -= 1;
        if ((tt = realloc(t, num_t_start * sizeof(*t))) == NULL) {
          printf("Nie udało się zaalokować pamięci");
          free(t); //*********************************************
          return 0;
        } else
          t = tt;
      } else
        t = tt;
      num_t = num_t_start;
      for (int i = 0; i < num_t; i++)
        startTarget(&(t[i]), num_t, i, color, num_c, level);
    }
    SDL_Delay(10);
  };
  free(t); // Zwolnienie pamięci celu
  return 0;
}