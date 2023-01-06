int Joystick::Loop(){





  
  if (joystickXValue != previousJoystickXValue || joystickYValue != previousJoystickYValue) {  // if either value has changed
    lastDebounceTime = currentTime;  // update the debounce time
  }
  if ((currentTime - lastDebounceTime) > debounceDelay) {  // if the debounce time has passed
    coordenadas.x.value_fixed = coordenadas.x.value - coordenadas.x.offset; //center the origin in (0,0)
    coordenadas.y.value_fixed = coordenadas.y.value - coordenadas.y.offset; //center the origin in (0,0)

    if(abs(coordenadas.x.value_read - coordenadas.x.value_fixed) > DIFFERENCE_ERROR_VALUE || 
       abs(coordenadas.y.value_read - coordenadas.y.value_fixed) > DIFFERENCE_ERROR_VALUE){
      coordenadas.x.value_read = coordenadas.x.value_fixed;
      coordenadas.y.value_read = coordenadas.y.value_fixed;
    }

    if(abs(coordenadas.x.value_fixed) < DIFFERENCE_ERROR_VALUE){ //The center gets the value
      coordenadas.x.value_fixed = 0;
    }

    if(abs(coordenadas.y.value_fixed) < DIFFERENCE_ERROR_VALUE){
      coordenadas.y.value_fixed = 0;
    }

    if(coordenadas.x.value_fixed == 0 && coordenadas.y.value_fixed == 0){
      direction = 0;
       } else {
      float angle = atan2(coordenadas.y.value_read,coordenadas.x.value_read); //calculate the angle to knows the direction
      if (angle > 0 && angle < PI / 2) {
        direction = 1; //derecha
      } else if (angle > PI / 2 && angle < PI) {
        direction = 2; //arriba
      } else if (angle < 0 && angle > -PI / 2) {
        direction = 1; //derecha
      } else {
        direction = 4; //abajo
      }
    }
  }
  // Direction(direction);
  return direction;
}
