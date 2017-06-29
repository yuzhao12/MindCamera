
from flask import Flask, jsonify, render_template, request
from werkzeug import secure_filename
#import inception_client

UPLOAD_FORDER = 'flaskr2/upload'
ALLOWED_ENTENSIONS = set(['txt','pdf','png','jpg','jpeg','gif'])

app = Flask(__name__,static_url_path='/flaskr2/static')
app.config['UPLOAD_FORDER'] = UPLOAD_FORDER
app.config['MAX_CONTENT_LENGTH'] = 16*1024*1024
def allowed_file(filename):
    return '.' in filename and \
    filename.rsplit('.',1)[1] in ALLOWED_EXTENSIONS

from flaskr2 import flaskr

