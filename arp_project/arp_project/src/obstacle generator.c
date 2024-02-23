// creating a new obstacle
struct obstacle *create_obstacle() {
  struct obstacle *obstacle = (struct obstacle *)malloc(sizeof(struct obstacle));
  obstacle->x = rand() % WINDOW_WIDTH;
  obstacle->y = rand() % WINDOW_HEIGHT;
  obstacle->radius = rand() % 100;
  return obstacle;
}

// sending the new obstacle to the server
void send_obstacle(struct obstacle *obstacle, int sockfd) {
  // sending the coordinates and radius of the obstacle
  send(sockfd, &obstacle->x, sizeof(obstacle->x), 0);
  send(sockfd, &obstacle->y, sizeof(obstacle->y), 0);
  send(sockfd, &obstacle->radius, sizeof(obstacle->radius), 0);
}
