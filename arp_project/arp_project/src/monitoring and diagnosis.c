// display the monitoring information
void display_monitoring_info() {
  // display the coordinates and velocity of the drone
  printf("x: %f, y: %f, z: %f, vx: %f, vy: %f, vz: %f\n", blackboard->drones[0].x, blackboard->drones[0].y, blackboard->drones[0].z, blackboard->drones[0].vx, blackboard->drones[0].vy, blackboard->drones[0].vz);

  // display the coordinates and radius of the obstacles
  for (int i = 0; i < blackboard->n_obstacles; i++) {
    printf("x: %f, y: %f, r: %f\n", blackboard->obstacles[i].x, blackboard->obstacles[i].y, blackboard->obstacles[i].radius);
  }
}

