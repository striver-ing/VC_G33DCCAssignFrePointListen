#include "com_client_DealTask.h"
#include "Controller.h"

/*
* Class:     com_client_DealTask
* Method:    getControllerAddr
* Signature: ()J
*/
JNIEXPORT jlong JNICALL Java_com_client_DealTask_getControllerAddr(JNIEnv *, jobject) {
	Controller* controller = new Controller();
	return (__int64)controller;
}

/*
* Class:     com_client_DealTask
* Method:    stopCurrentTask
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_com_client_DealTask_stopCurrentTask(JNIEnv *, jobject, jlong controllerAddr) {
	Controller* controller = (Controller*)controllerAddr;
	controller->stopRecording();
}

/*
* Class:     com_client_DealTask
* Method:    doTask
* Signature: (JLjava/lang/String;IILjava/lang/String;I)V
*/
JNIEXPORT void JNICALL Java_com_client_DealTask_doTask(JNIEnv *env, jobject obj, jlong controllerAddr, jstring receiverIp, jint receiverPort, jint frequence, jstring filePath, jint fileTotalTime) {
	const char* serialNumber = env->GetStringUTFChars(receiverIp, 0);
	const char* path = env->GetStringUTFChars(filePath, 0);
	int channel = receiverPort;

	Controller* controller = (Controller*)controllerAddr;
	controller->doTask(serialNumber, channel, frequence, path, fileTotalTime);

	env->ReleaseStringUTFChars(receiverIp, serialNumber);
	env->ReleaseStringUTFChars(filePath, path);
}