
#include "LeapControl.h"
int pinchStatus=0, prevpinchStatus=0;
int grabStatus=0, prevgrabStatus=0;

void SampleListener::onInit(const Controller& controller) {
	/*std::cout << "Initialized" << std::endl;*/
}

void SampleListener::onConnect(const Controller& controller) {
	/*std::cout << "Connected" << std::endl;*/
	controller.enableGesture(Gesture::TYPE_CIRCLE);
	controller.enableGesture(Gesture::TYPE_KEY_TAP);
	controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Gesture::TYPE_SWIPE);
}

void SampleListener::onDisconnect(const Controller& controller) {
	/*std::cout << "Disconnected" << std::endl;*/
}

void SampleListener::onExit(const Controller& controller) {
	/*std::cout << "Exited" << std::endl;*/
}

void SampleListener::onFrame(const Controller& controller) {
	// Get the most recent frame and report some basic information
	const Frame frame = controller.frame();

	HandList hands = frame.hands();
	for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
		// Get the first hand
		const Hand hand = *hl;
		const Vector stable = hand.stabilizedPalmPosition();
		const Vector direction = hand.direction();

		if (hands[0].pinchStrength() > 0.5){
			prevpinchStatus = pinchStatus;
			pinchStatus = 1;
		}
		else{
			prevpinchStatus = pinchStatus;
			pinchStatus = 0;
		}
		if (hands[0].grabStrength() > 0.8){
			prevgrabStatus = grabStatus;
			grabStatus = 1;
		}
		else{
			prevgrabStatus = grabStatus;
			grabStatus = 0;
		}

		if (prevpinchStatus == 0 && pinchStatus == 1){
			keybd_event(90, 0, 0, 0);
		}
		else if (prevpinchStatus == 1 && pinchStatus == 0){
			keybd_event(90, 0, KEYEVENTF_KEYUP, 0);
		}
		if (prevgrabStatus == 0 && grabStatus == 1){
			keybd_event(88, 0, 0, 0);
		}
		else if (prevgrabStatus == 1 && grabStatus == 0){
			keybd_event(88, 0, KEYEVENTF_KEYUP, 0);
		}


		if (stable.x > 10){
			keybd_event(VK_RIGHT, 0, 0, 0);
		}
		else if (stable.x < -10){
			keybd_event(VK_LEFT, 0, 0, 0);
		}
		else{
			keybd_event(VK_LEFT, 0, KEYEVENTF_KEYUP, 0);
			keybd_event(VK_RIGHT, 0, KEYEVENTF_KEYUP, 0);
		}
		if (stable.y > 200){
			keybd_event(VK_UP, 0, 0, 0);
		}
		else if (stable.y < 180){
			keybd_event(VK_DOWN, 0, 0, 0);
		}
		else{
			keybd_event(VK_UP, 0, KEYEVENTF_KEYUP, 0);
			keybd_event(VK_DOWN, 0, KEYEVENTF_KEYUP, 0);
		}


	}
}

void SampleListener::onFocusGained(const Controller& controller) {
	/*std::cout << "Focus Gained" << std::endl;*/
}

void SampleListener::onFocusLost(const Controller& controller) {
	/*std::cout << "Focus Lost" << std::endl;*/
}

void SampleListener::onDeviceChange(const Controller& controller) {
	/*std::cout << "Device Changed" << std::endl;*/
}

void SampleListener::onServiceConnect(const Controller& controller) {
	/*std::cout << "Service Connected" << std::endl;*/
}

void SampleListener::onServiceDisconnect(const Controller& controller) {
	/*std::cout << "Service Disconnected" << std::endl;*/
}
