void update_screen_time() {

  if (!DCCpp::IsPowerOnMain) {
    display.clearDisplay();
    display.setCursor(20,10);
    display.println("Pwr Off");
  } else {
    // Loco info
    display.clearDisplay();
    display.setCursor(20,10);
    display.println("Loc: " + String(locos[locoIndex].locoId));
    display.setCursor(20,25);
    display.println("Spd: " + String(locos[locoIndex].locoSpeed));
    display.setCursor(20,40);
    display.println("Dir: " + String(locos[locoIndex].locoDirectionForward));
  }

   // Finally display
  display.display();
}
