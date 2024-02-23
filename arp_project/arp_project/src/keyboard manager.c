// getting input
int get_input() {
  int c = getch();
  switch (c) {
    case 'w':
      return UP;
    case 's':
      return DOWN;
    case 'a':
      return LEFT;
    case 'd':
      return RIGHT;
    case 'q':
      return QUIT;
    default:
      return NONE;
  }
}

// sending input to the drone
void send_input(int input, int sockfd) {
  send(sockfd, &input, sizeof(input), 0);
}
