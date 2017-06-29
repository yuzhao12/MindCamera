# all the imports
import os
from flask import Flask, request, session, g, redirect, url_for, \
     abort, render_template, flash, jsonify
import base64
import cv2
import numpy as np
import ctypes
import time
import re
import httplib, urllib
import json
from review import wrap_Doreview, wrap_ReadVocabulary, wrap_ReadTfidf, wrap_ReadInvertedList, wrap_ReadIdfMat, wrap_ReadFileName
import sys
from flaskr2 import app

reload(sys)
sys.setdefaultencoding('utf-8')
# create our little application :)
#def allowed_file(filename):
#app = Flask(__name__)
#app.config.from_object(__name__)


@app.before_first_request
def init():
	print wrap_ReadVocabulary('./flaskr2/static/trainsave/S_vocabulary.xml')
	print wrap_ReadTfidf('./flaskr2/static/trainsave/SwordFre_10000.xml')
	print wrap_ReadInvertedList('./flaskr2/static/trainsave/SinvertedList_10000.txt')
	print wrap_ReadIdfMat('./flaskr2/static/trainsave/IDF_10000.xml')
	print wrap_ReadFileName('./flaskr2/static/trainsave/filename.txt')


localtime=""
imagefile = './flaskr2/static/img/'
url='./static/img/'
global tmpImgUrl
global tmpImgUrl2

def grabcut(rect,filename):
	img = cv2.imread(filename)
	img2 = img.copy()
	mask = np.zeros(img.shape[:2],dtype = np.uint8) # mask initialized to PR_BG
	output = np.zeros(img.shape,np.uint8)           # output image to be shown    
	bgdmodel = np.zeros((1,65),np.float64)
	fgdmodel = np.zeros((1,65),np.float64)
	cv2.grabCut(img2,mask,rect,bgdmodel,fgdmodel,1,cv2.GC_INIT_WITH_RECT)
	mask2 = np.where((mask==1) + (mask==3),255,0).astype('uint8')
	img3=img2[rect[1]:rect[1]+rect[3],rect[0]:rect[0]+rect[2]]
	mask3=mask2[rect[1]:rect[1]+rect[3],rect[0]:rect[0]+rect[2]]
	output = cv2.bitwise_and(img3,img3,mask=mask3) 
	cv2.imwrite(imagefile+localtime+"mask.jpg",mask3)
	return output


def grabcur_mask(filename):
	mask_re = cv2.imread(filename[0],-1)    #base64
	img2 = cv2.imread(filename[1])         #resize.jpg
	mask_org =cv2.imread(filename[2],0)      #mask.jpg
	mask = np.where((mask_org>20),3,2).astype('uint8')
	mask1 = np.where((mask_re[:,:,0]>10)|(mask_re[:,:,1]>10)|(mask_re[:,:,2]>10)&(mask_re[:,:,3]>200),1,mask).astype('uint8')
	mask2 = np.where((mask_re[:,:,0]<10)&(mask_re[:,:,1]<10)&(mask_re[:,:,2]<10)&(mask_re[:,:,3]>200),0,mask1).astype('uint8')
	bgdmodel = np.zeros((1,65),np.float64)
	fgdmodel = np.zeros((1,65),np.float64)
	rect=(0,0,img2.shape[1],img2.shape[0])
	cv2.grabCut(img2,mask2,rect,bgdmodel,fgdmodel,2,cv2.GC_INIT_WITH_MASK)
	mask5 = np.where((mask2==1)+(mask2==3),255,0).astype('uint8')
	output = cv2.bitwise_and(img2,img2,mask=mask5) 
	cv2.imwrite(filename[2],mask5)
	cv2.imwrite(filename[3],output)    #matting.jpg


def fusion(filename,rect):
	so=ctypes.cdll.LoadLibrary
	lib=so("./flaskr2/../lib/libfusion.so")
	lib.doClone(filename[1],filename[2],filename[0],filename[3],rect[0],rect[1],rect[2],rect[3])
@app.route('/flaskr2/',methods=['GET','POST'])
@app.route('/flaskr2/index',methods=['GET','POST'])
def show_entries():
	return render_template('index.html')
@app.route('/flaskr2/draft')
def draft():
    return render_template("draft.html")
@app.route('/flaskr2/draftResult', methods = ['POST'])
def Controldddddd():
	
	res1=[]
		
	i=0
	while i<len(dict(request.form).values()[0]):
		json_data = {key:dict(request.form)[key][i] for key in dict(request.form)}
		imgData = base64.b64decode(json_data.get('value[]'))	
		localtime = time.strftime("%Y%m%d%H%M%S", time.localtime())			
		f = open('./flaskr2/static/img/'+localtime+'.jpg','wb')
		f.write(imgData)
		f.close()
		result1=wrap_Doreview('./flaskr2/static/img/'+localtime+'.jpg')
		res=result1.split(" ")
		res1.append(res)
		i=i+1
		
	print res1
	return jsonify(result = res1)


@app.route('/flaskr2/draftAndroid', methods = ['POST'])
def draftAndroid():
	imgData = base64.b64decode(request.json['value'])
	localtime = time.strftime("%Y%m%d%H%M%S", time.localtime())
	f = open('./flaskr2/static/img/'+localtime+'.jpg','wb')
	f.write(imgData)
	f.close()	
	result1=wrap_Doreview('./flaskr2/static/img/'+localtime+'.jpg')
	res=result1.split(" ")

        print res
        return jsonify(result = res)

@app.route('/flaskr2/result1', methods = ['POST'])
def Control():
	global tmpImgUrl
	# internet url copy to local
	tmpImgUrl = request.form.get('value')
	return "success"
	
@app.route('/flaskr2/result11', methods = ['POST'])
def Control11():
	global tmpImgUrl2
	# internet url copy to local
	tmpImgUrl2 = request.form.get('value')
	return "success"	
	
@app.route('/flaskr2/result4', methods = ['POST'])
def SelfControl():
	global tmpImgUrl
	imgData = base64.b64decode(request.form.get('value'))
	localtime = time.strftime("%Y%m%d%H%M%S", time.localtime())
	f = open('./flaskr2/static/img/'+localtime+'self.jpg','wb')
	f.write(imgData)
	f.close()
	tmpImgUrl = './flaskr2/static/img/'+localtime+'self.jpg'
	return jsonify(result = tmpImgUrl)

@app.route('/flaskr2/result41', methods = ['POST'])
def SelfControl1():
	global tmpImgUrl2
	imgData = base64.b64decode(request.form.get('value'))
	localtime = time.strftime("%Y%m%d%H%M%S", time.localtime())
	f = open('./flaskr2/static/img/'+localtime+'self.jpg','wb')
	f.write(imgData)
	f.close()
	tmpImgUrl2 = './flaskr2/static/img/'+localtime+'self.jpg'
	return jsonify(result = tmpImgUrl2)
	
@app.route('/flaskr2/tmp', methods = ['POST'])
def sendURL():
	global tmpImgUrl
#	tmpImgUrl = '.'+tmpImgUrl.split('flaskr2')[1]
	return jsonify(result=tmpImgUrl)
	
@app.route('/flaskr2/tmp1', methods = ['POST'])
def sendURL1():
	global tmpImgUrl2
#	tmpImgUrl2 = '.'+tmpImgUrl2.split('flaskr2')[1]
	return jsonify(result=tmpImgUrl2)
	
@app.route('/flaskr2/imageCombine')	
def imageCombine1():
	return render_template('imageCombine.html')
@app.route('/flaskr2/getdata',methods=['POST','GET'])
def get_data():
	global localtime
	localtime = time.strftime("%Y%m%d%H%M%S", time.localtime())
	filename=imagefile+localtime+'input.jpg'
	#print request.headers 
	#print request.json
	if request.form:

		data=request.form.get('imagedata')
		data_tmp = eval(data)
		#print data_tmp['x1']
		imgData=base64.b64decode(request.form.get('value'))
	
	else:
		data = request.json['imagedata']
		#print data
		data_tmp = json.loads(data)
		#print data_tmp
		#print data_tmp['value']
		imgData=base64.b64decode(data_tmp['value'])
		#print imgData
		fileImg = open('1.png','wb')
		fileImg.write(imgData)
		fileImg.close()
	uploadimg = open(filename,'wb')
	uploadimg.write(imgData)
	uploadimg.close()
	img1 = cv2.imread(filename)
	shape=img1.shape
	max_leg=max(shape)
	ratio=1.0
	if(max_leg>600):
		ratio=600.0/max_leg
		img1=cv2.resize(img1,(int(shape[1]*ratio),int(shape[0]*ratio)))
		cv2.imwrite(filename,img1)
	if data_tmp['x1']<0:
		data_tmp['x1']=0 
	if data_tmp['y']<0:
		data_tmp['y']=0
	if data_tmp['height']>shape[0]:
		data_tmp['height']=shape[0]
	if data_tmp['width']>shape[1]:
		data_tmp['width']=shape[1]
	rect=(int(data_tmp['x1']*ratio),int(data_tmp['y']*ratio),int(data_tmp['width']*ratio),int(data_tmp['height']*ratio))
	img2=img1[rect[1]:rect[1]+rect[3],rect[0]:rect[0]+rect[2]]
	cv2.imwrite(imagefile+localtime+"resize.jpg",img2)
	dst=grabcut(rect,filename)
	cv2.imwrite(imagefile+localtime+"matting.jpg",dst)
	#result={"state":200,'message':"ok",'result':"http://10.108.126.130:5000/static/img/matting.jpg"}
	result={"state":200,'message':"ok",'result':url+localtime+'matting.jpg','result1':url+localtime+'resize.jpg'}
	return jsonify(result)
@app.route('/flaskr2/further',methods=['POST','GET'])
def further():
	global localtime
	filename=(imagefile+localtime+'remask.jpg',imagefile+localtime+"resize.jpg",imagefile+localtime+"mask.jpg",imagefile+localtime+'mattings.jpg')
	imgData=base64.b64decode(request.form.get('value'))
	uploadimg = open(filename[0],'wb')
	uploadimg.write(imgData)
	uploadimg.close()
	grabcur_mask(filename)
	result={"state":200,'message':"ok",'result':url+localtime+'mattings.jpg'}
	return jsonify(result)









@app.route('/flaskr2/getimgdata',methods=['POST','GET'])
def get_imagedata():
	if request.form:
		imgData=base64.b64decode(request.form.get('value'))
		k=0
		while(k<(len(request.form.keys())/6)):
			if(request.form.get('imagedata['+str(k)+'][id]')):
				fID = request.form.get('imagedata['+str(k)+'][id]').encode("utf-8")
				#print type(fID.encode("utf-8"))
				#print fID
				filename=(imagefile+fID+'background.jpg',imagefile+fID+"resize.jpg",imagefile+fID+"mask.jpg",imagefile+fID+'fusion.jpg')
			if(request.form.get('imagedata['+str(k)+'][x1]')):
				xx = request.form.get('imagedata['+str(k)+'][x1]').encode("utf-8")
				#print type(xx)
			if(request.form.get('imagedata['+str(k)+'][y]')):
				yy = request.form.get('imagedata['+str(k)+'][y]').encode("utf-8")
				
				
			if(request.form.get('imagedata['+str(k)+'][width]')):
				width = request.form.get('imagedata['+str(k)+'][width]').encode("utf-8")
			if(request.form.get('imagedata['+str(k)+'][height]')):
				height = request.form.get('imagedata['+str(k)+'][height]').encode("utf-8")
			if(request.form.get('imagedata['+str(k)+'][rotate]')):
				rotate = request.form.get('imagedata['+str(k)+'][rotate]').encode("utf-8")
			
			uploadimg = open(filename[0],'wb')
			uploadimg.write(imgData)
			uploadimg.close()
			
			img1 = cv2.imread(filename[1])
			shape=img1.shape
			
			#if (int(xx)<0):
			if (int(round(float(xx)))<0):
				xx=0
			if (int(round(float(yy)))<0):
				yy=0
			r1=float(height)/shape[0]
			rect=(int(round(float(xx))),int(round(float(yy))),ctypes.c_float(r1),int(rotate)) 
			
			fusion(filename,rect)
			f = open(filename[3],'rb')
			imgData = f.read()
			f.close()
			k=k+1
		result={"state":200,'message':"ok",'result':url+fID+"fusion.jpg"}
		return jsonify(result)
	#else:
	#	print "Android Combine"
	#	imgData=base64.b64decode(request.json['value'])
	#	data=request.json['imagedata']
	#	data_tmp = data
	
@app.route('/flaskr2/result3', methods = ['POST'])
def imageComb():
	# internet url copy to local
	global tmpImgUrl
	tmpImgUrl = request.form.get('value')
	localtime = time.strftime("%Y%m%d%H%M%S", time.localtime())
	localUrl = './flaskr2/static/img/'+localtime+'text.jpg'
	urllib.urlretrieve(tmpImgUrl,localUrl)
	tmpImgUrl = localUrl
	return jsonify(result = tmpImgUrl)
	
@app.route('/flaskr2/result31', methods = ['POST'])
def imageComb1():
	global tmpImgUrl2
	# internet url copy to local
	tmpImgUrl2 = request.form.get('value')
	localtime = time.strftime("%Y%m%d%H%M%S", time.localtime())
	localUrl = './flaskr2/static/img/'+localtime+'text.jpg'
	urllib.urlretrieve(tmpImgUrl2,localUrl)
	tmpImgUrl2 = localUrl
	return jsonify(result = tmpImgUrl2)

@app.route('/flaskr2/resAndroid',methods=['POST','GET'])
def testAndroid():
	headers = {
		'Content-Type': 'multipart/form-data',
		'Ocp-Apim-Subscription-Key': 'b4f268f76170485ebc1e78a045554fae',
	}
	imgURL1=[]
	imgURL2=[]
	typeJson = json.loads(request.json['queryexpression'])['type']
	if(typeJson=='0'):
		values={}
		values['q'] = json.loads(request.json['queryexpression'])['background']	
		params = urllib.urlencode(values)
		#print params
		#try:
		conn = httplib.HTTPSConnection('api.cognitive.microsoft.com')
		conn.request("POST", "/bing/v5.0/images/search?%s" % params, "{body}", headers)
		response = conn.getresponse()

		data = response.read()
		ImgUrl = []
	
		hjsons = json.loads(data)
		ImgLength = len(hjsons['value'])
		i=0
		while(i<ImgLength):
			ImgUrl.append(hjsons['value'][i]['contentUrl'])
			#localUrl = './flaskr2/static/img/text'+str(i)+'.jpg'
			#urllib.urlretrieve(hjsons['value'][i]['contentUrl'],localUrl)
			#ImgUrl.append(localUrl)
			i=i+1
		i=0
		while(i<ImgLength):
			ImgUrl.append(hjsons['value'][i]['thumbnailUrl'])
			i=i+1
		imgURL1.append(ImgUrl)		

	if(typeJson=='1'):
		values={}
		values['q'] = json.loads(request.json['queryexpression'])['good']	
		params = urllib.urlencode(values)

		conn = httplib.HTTPSConnection('api.cognitive.microsoft.com')
		conn.request("POST", "/bing/v5.0/images/search?%s" % params, "{body}", headers)
		response = conn.getresponse()

		data = response.read()
		ImgUrlA = []

		hjsons = json.loads(data)
		ImgLength = len(hjsons['value'])

		i=0
		while(i<ImgLength):
			ImgUrlA.append(hjsons['value'][i]['contentUrl'])
			#localUrl = './flaskr2/static/img/textA'+str(i)+'.jpg'
			#urllib.urlretrieve(hjsons['value'][i]['contentUrl'],localUrl)
			#ImgUrlA.append(localUrl)
			i=i+1
		i=0
		while(i<ImgLength):
			ImgUrlA.append(hjsons['value'][i]['thumbnailUrl'])
			i=i+1	
		imgURL2.append(ImgUrlA)
	return jsonify(result = imgURL1,result1 = imgURL2)
		
@app.route('/flaskr2/result2', methods = ['POST'])
def Control2():
	
	headers = {
		# Request headers
		'Content-Type': 'multipart/form-data',
		'Ocp-Apim-Subscription-Key': 'b4f268f76170485ebc1e78a045554fae',
	}	
	#values={}
	#print request.form.get('queryexpression[0][background]')

	#json_data = {key:dict(request.form)[key][0] for key in dict(request.form)}
	#print json_data
	imgURL1=[]
	imgURL2 = []
	k=0
	while(k<len(request.form.keys())):
		if(request.form.get('queryexpression['+str(k)+'][background]')):
			values={}
			values['q'] = request.form.get('queryexpression['+str(k)+'][background]')
			params = urllib.urlencode(values)
			#print params
			#try:
			conn = httplib.HTTPSConnection('api.cognitive.microsoft.com')
			conn.request("POST", "/bing/v5.0/images/search?%s" % params, "{body}", headers)
			response = conn.getresponse()
	
			data = response.read()
			ImgUrl = []
		
			hjsons = json.loads(data)
			ImgLength = len(hjsons['value'])
			i=0
			while(i<ImgLength):
				ImgUrl.append(hjsons['value'][i]['contentUrl'])
				#localUrl = './flaskr2/static/img/text'+str(i)+'.jpg'
				#urllib.urlretrieve(hjsons['value'][i]['contentUrl'],localUrl)
				#ImgUrl.append(localUrl)
				i=i+1
			i=0
			while(i<ImgLength):
				ImgUrl.append(hjsons['value'][i]['thumbnailUrl'])
				i=i+1
			imgURL1.append(ImgUrl)		
		elif(request.form.get('queryexpression['+str(k)+'][good]')):
			values={}
			values['q'] = request.form.get('queryexpression['+str(k)+'][good]')
			params = urllib.urlencode(values)
	
			conn = httplib.HTTPSConnection('api.cognitive.microsoft.com')
			conn.request("POST", "/bing/v5.0/images/search?%s" % params, "{body}", headers)
			response = conn.getresponse()
	
			data = response.read()
			ImgUrlA = []
	
			hjsons = json.loads(data)
			ImgLength = len(hjsons['value'])
	
			i=0
			while(i<ImgLength):
				ImgUrlA.append(hjsons['value'][i]['contentUrl'])
				#localUrl = './flaskr2/static/img/textA'+str(i)+'.jpg'
				#urllib.urlretrieve(hjsons['value'][i]['contentUrl'],localUrl)
				#ImgUrlA.append(localUrl)
				i=i+1
			i=0
			while(i<ImgLength):
				ImgUrlA.append(hjsons['value'][i]['thumbnailUrl'])
				i=i+1	
			imgURL2.append(ImgUrlA)
		k=k+1
	#conn.close()
	return jsonify(result = imgURL1,result1 = imgURL2)	

