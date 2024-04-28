import RPi.GPIO as GPIO
import time
import threading
import json
import picamera
import sys

buzz = 27
servoPin = 4
melody = [262,294,330,349,392,440,494,524,1,1]

rowPin = [21,20,16,26]
colPin = [19,13,6,5]
list = ['123A','456I','789J','*0#Q']

# 저장된 비밀번호
with open('password.json', 'r') as json_pswd:
	password = json.load(json_pswd)
pswd = password['pswd']

chBuf = ''
th_stop = False

# 촬영설정
camera = picamera.PiCamera()
camera.resolution = (1920,1440)
camera.framerate = 30
path = 'image/'	# 저장경로

GPIO.setmode(GPIO.BCM)
GPIO.setup(rowPin, GPIO.OUT, initial=0)
GPIO.setup(buzz, GPIO.OUT, initial=0)
GPIO.setup(servoPin, GPIO.OUT, initial=0)
GPIO.setup(colPin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

buz = GPIO.PWM(buzz, 400)
servo = GPIO.PWM(servoPin, 50)
servo.start(2)


# 키패드
def th_keypad():
	global chBuf
	while True:
		if th_stop:
			break
		for i in range(4):
			GPIO.output(rowPin[i], 1)
			for j in range(4):
				if GPIO.input(colPin[j]):
					time.sleep(0.05)
					if GPIO.input(colPin[j]):
						chBuf = list[i][j]
						# print(chBuf)
						dcnt = 0
						while True:
							time.sleep(0.01)
							if not GPIO.input(colPin[j]):
								dcnt=dcnt+1
							if dcnt>5:
								break
			GPIO.output(rowPin[i], 0)


# 사진 촬영
def photo_():
    print('촬영중..')
    tim = time.localtime()
    name = str(tim.tm_year)+'-'+str(tim.tm_mon)+'-'+str(tim.tm_mday)+'_'+\
		str(tim.tm_hour)+':'+str(tim.tm_min)+':'+str(tim.tm_sec)+'.jpg'
    img = path + name
    camera.capture(img)
    print(img,'저장됨')


# 입력결과음
def sound_open():
	buz.start(50)
	for i in range(3):
		buz.ChangeFrequency(melody[i*2]*2)
		time.sleep(0.3)
	buz.stop()

def sound_close():
	buz.ChangeFrequency(melody[2]*2)
	buz.start(50)
	time.sleep(0.2)
	buz.ChangeFrequency(melody[0]*2)
	time.sleep(0.2)

def sound_false():
	for i in range(4):
		buz.start(50)
		buz.ChangeFrequency(300)
		time.sleep(0.08)
		buz.stop()
		time.sleep(0.08)

def sound_change():
	buz.ChangeFrequency(500)
	buz.start(50)
	time.sleep(0.4)
	buz.stop()


# 메인
try:
	th = threading.Thread(target = th_keypad)
	th.start()	# 키패드 스레드 시작
	mode = 'n'
	pswd_in = ''
	pswd_new = ''
	pswd_new2 = ''
	cnt = 0

	while True:
		if chBuf: # 키패드 입력버퍼
			# 입력모드
			if chBuf == '*':
				if mode == 'n':
					mode = 'in'
					pswd_in = ''
					print('비밀번호 입력모드')
					for i in range(2):
						buz.ChangeFrequency(500)
						buz.start(50)
						time.sleep(0.08)
						buz.stop()
						time.sleep(0.08)

				elif mode == 'in':
					if pswd_in == pswd:
						print('\n문이 열렸습니다')
						servo.ChangeDutyCycle(6.9)
						buz.start(50)
						sound_open()
						cnt = 0
					else:
						cnt = cnt+1
						print('\n비밀번호를 ' + str(cnt) + '회 틀렸습니다')
						servo.ChangeDutyCycle(2.0)
						sound_false()
						if cnt > 2:
							photo_()
					pswd_in = ''
					mode = 'n'

			# 비밀번호 변경모드
			elif chBuf == 'A':
				if mode == 'n':
					print('비밀번호 변경모드 - 이전 비밀번호 입력')
					pswd_in = ''
					pswd_new = ''
					pswd_new2 = ''
					sound_change()
					mode = 'bef'

				elif mode == 'bef':
					if pswd_in == pswd:
						cnt = 0
						print('\n새 비밀번호 입력')
						sound_change()
						mode = 'ch'
					else:
						cnt += 1
						print('\n비밀번호를', cnt, '회 틀렸습니다')
						sound_false()
						if cnt > 2:
							photo_()
						mode = 'n'

				elif mode == 'ch':
					print('\n한번더 입력')
					sound_change()
					mode = 'ch2'

				elif mode == 'ch2':
					if pswd_new == pswd_new2:
						print('\n비밀번호 변경완료')
						for i in range(3):
							buz.ChangeFrequency(500)
							buz.start(50)
							time.sleep(0.2)
							buz.stop()
							time.sleep(0.1)
						pswd = pswd_new
						password['pswd'] = pswd_new
						with open('password.json', 'w') as json_pswd:
							json.dump(password, json_pswd)
						mode = 'n'
					else:
						print('\n일치하지 않습니다')
						sound_false()
						mode = 'n'

			# 종료
			elif chBuf == 'Q':
				break

			# 문닫힘
			elif chBuf == '#' and mode == 'n':
				print('문이 잠깁니다')
				servo.ChangeDutyCycle(2.0)
				sound_close()

			# 숫자입력
			else:
				if mode != 'n' and chBuf >= '0' and chBuf <= '9':
					if mode == 'ch':
						pswd_new = pswd_new + chBuf
					elif mode == 'ch2':
						pswd_new2 = pswd_new2 + chBuf
					else:
						pswd_in = pswd_in + chBuf
					print('*',end='')
					sys.stdout.flush()
					buz.ChangeFrequency(500)
					buz.start(50)

		chBuf = ''
		time.sleep(0.1)
		buz.stop()


except KeyboardInterrupt:
	th_stop = True
finally:
	th_stop = True
	th.join()
	GPIO.cleanup()
	print('종료')
