
document.write("<script language=javascript src='./static/example/crop-avatar/js/jtopo-min.js'></script>");
(function (factory) {
  if (typeof define === 'function' && define.amd) {
    define(['jquery'], factory);
  } else if (typeof exports === 'object') {
    // Node / CommonJS
    factory(require('jquery'));
  } else {
    factory(jQuery);
  }
})


(function ($) {

  'use strict';
//  var sendURL1='./static/Imgs/75.jpg';
  var frontURL;
  
	var console = window.console || { log: function () {} };

  function CropAvatar($element) {
    this.$container = $element;

    this.$avatarView = this.$container.find('.avatar-view');
    this.$avatar = this.$avatarView.find('img');
    this.$avatarModal = this.$container.find('#avatar-modal');
    this.$loading = this.$container.find('.loading');

    this.$avatarForm = this.$avatarModal.find('.avatar-form');
    this.$avatarUpload = this.$avatarForm.find('.avatar-upload');
    this.$avatarSrc = this.$avatarForm.find('.avatar-src');
    this.$avatarData = this.$avatarForm.find('.avatar-data');
    this.$avatarInput = this.$avatarForm.find('.avatar-input');
    this.$avatarSave = this.$avatarForm.find('.avatar-save');
    this.$avatarBtns = this.$avatarForm.find('.avatar-btns');

    this.$avatarWrapper = this.$avatarModal.find('.avatar-wrapper');
    this.$avatarPreview = this.$avatarModal.find('.avatar-preview');

    this.init();
  }

  function getBase64Image(img) {
        var canvas = document.createElement("canvas");
        canvas.width = img.width;
        canvas.height = img.height;
        var ctx = canvas.getContext("2d");
        ctx.drawImage(img, 0, 0, img.width, img.height);
        var ext = img.src.substring(img.src.lastIndexOf(".")+1).toLowerCase();
        var dataURL = canvas.toDataURL("image/"+ext);
        return dataURL;
	}


  
  CropAvatar.prototype = {
    constructor: CropAvatar,
	
    support: 
        {
      fileList: !!$('<input type="file">').prop('files'),
      blobURLs: !!window.URL && URL.createObjectURL,
      formData: !!window.FormData
    },

    init: function () {
      this.support.datauri = this.support.fileList && this.support.blobURLs;

      if (!this.support.formData) {
        this.initIframe();
      }

      this.initTooltip();
      this.initModal();
      this.addListener();
    },

    addListener: function () {
      this.$avatarView.on('click', $.proxy(this.click, this));
      this.$avatarInput.on('click', $.proxy(this.change, this));
      this.$avatarForm.on('submit', $.proxy(this.submit, this));
      this.$avatarBtns.on('click', $.proxy(this.rotate, this));
    },

    initTooltip: function () {
      this.$avatarView.tooltip({
        placement: 'bottom'
      });
    },

    initModal: function () {
      this.$avatarModal.modal({
        show: false
      });
    },
	
//url下标需要改
    initPreview: function () {
      var url = frontURL;
      this.$avatarPreview.empty().html('<img src="' + url + '">');
    },

    initIframe: function () {
      var target = 'upload-iframe-' + (new Date()).getTime(),
          $iframe = $('<iframe>').attr({
            name: target,
            src: ''
          }),
          _this = this;

      // Ready ifrmae
      $iframe.one('load', function () {

        // respond response
        $iframe.on('load', function () {
          var data;

          try {
            data = $(this).contents().find('body').text();
          } catch (e) {
            console.log(e.message);
          }

          if (data) {
            try {
              data = $.parseJSON(data);
            } catch (e) {
              console.log(e.message);
            }

            _this.submitDone(data);
          } else {
            _this.submitFail('Image upload failed!');
          }

          _this.submitEnd();

        });
      });

      this.$iframe = $iframe;
      this.$avatarForm.attr('target', target).after($iframe.hide());
    },

	click: function () {
		this.$avatarModal.modal('show');
		this.initPreview();
	
    	},


    submit: function () {

      if (!this.$avatarSrc.val() && !this.$avatarInput.val()) {
	
        
		return false;
      }
     
		this.ajaxUpload();
		return false;
    },

    rotate: function (e) {
      var data;

      if (this.active) {
        data = $(e.target).data();

        if (data.method) {
          this.$img.cropper(data.method, data.option);
        }
      }
    },

    isImageFile: function (file) {
      if (file.type) {
        return /^image\/\w+$/.test(file.type);
      } else {
        return /\.(jpg|jpeg|png|gif)$/.test(file);
      }
    },

    change: function () {
      var _this = this;
	var substr = "./static/Imgs/";
	var sublocal = "blob";
	if(window.cropimg.indexOf(substr)>=0){  
		frontURL = window.cropimg;
		_this.url = frontURL;
                                        _this.$img = $('<img src="' + _this.url + '">');
                                        _this.$avatarWrapper.empty().html(_this.$img);
        _this.$img.cropper({
          preview: _this.$avatarPreview.selector,
          strict: false,
          crop: function (data) {
            var json = [
                  '{"x1":' + data.x,
                  '"y":' + data.y,
                  '"height":' + data.height,
                  '"width":' + data.width,
                  '"rotate":' + data.rotate + '}'
                ].join();

            _this.$avatarData.val(json);
          }
        });

        _this.active = true;
  
        }
	else if(window.cropimg.indexOf(sublocal)==0){
                frontURL = window.cropimg;
                _this.url = frontURL;
                                        _this.$img = $('<img src="' + _this.url + '">');
                                        _this.$avatarWrapper.empty().html(_this.$img);
        _this.$img.cropper({
          preview: _this.$avatarPreview.selector,
          strict: false,
          crop: function (data) {
            var json = [
                  '{"x1":' + data.x,
                  '"y":' + data.y,
                  '"height":' + data.height,
                  '"width":' + data.width,
                  '"rotate":' + data.rotate + '}'
                ].join();

            _this.$avatarData.val(json);
          }
        });

        _this.active = true;


	}
	

	else{  
        	$.ajax({
                                url: '/flaskr2/result3',
                                method: 'POST',
                                //    contentType:"charset=utf-8",
                                data: {"value": window.cropimg},
                                beforeSend: function (){
                                        $("#loadingA").css("top",$(document).scrollTop()+'px');
                                        $("#loadingA").fadeIn();

                                },
                                success: function(data) {

                                        frontURL = data.result;
                                        var pattern  = '/flaskr2';
                                        frontURL = frontURL.replace(new RegExp(pattern), "");
                                        _this.url = frontURL;
                                        _this.$img = $('<img src="' + _this.url + '">');
                                        _this.$avatarWrapper.empty().html(_this.$img);
        _this.$img.cropper({
          preview: _this.$avatarPreview.selector,
          strict: false,
          crop: function (data) {
            var json = [
                  '{"x1":' + data.x,
                  '"y":' + data.y,
                  '"height":' + data.height,
                  '"width":' + data.width,
                  '"rotate":' + data.rotate + '}'
                ].join();

            _this.$avatarData.val(json);
          }
        });

        _this.active = true;
                                },
                                error: function (XMLHttpRequest, textStatus, errorThrown) {
                                        alert(errorThrown);
                                },
                                complete: function(){
                                        $("#loadingA").css("top",0);
                                        $("#loadingA").fadeOut();
                                }
                });
        }  	 


    },

    stopCropper: function () {
      if (this.active) {
        this.$img.cropper('destroy');
        this.$img.remove();
        this.active = false;
      }
    },

    ajaxUpload: function () {
		
		var url = this.$avatarForm.attr('action');
		var _this = this;
		
		var image = new Image();
		//image.src = sendURL;
		image.src = frontURL;
		image.onload = function(){
			var base = getBase64Image(image);
			var base64 = base.split(',')[1];
			var imagedata = _this.$avatarData.val();
           
            $.ajax({
                url: url,
                method: 'POST',
                data: { "value": base64,"imagedata": imagedata},
                 dataType: 'json',
                //processData: false,
                //contentType: false,

                beforeSend: function () {
                    _this.submitStart();
                },

               success: function (data) {
                    _this.submitDone(data);
                },

               error: function (XMLHttpRequest, textStatus, errorThrown) {
					alert(errorThrown);
                    _this.submitFail(textStatus || errorThrown);
                },
               
                complete: function () {
                    _this.submitEnd();
                }

            });
		}
    },

    syncUpload: function () {
      this.$avatarSave.click();
    },

    submitStart: function () {
	this.$loading.css("top",$(document).scrollTop()+'px');
      this.$loading.fadeIn();
    },

    submitDone: function (data) {

      if ($.isPlainObject(data) && data.state === 200) {
        if (data.result) {
          this.url = data.result;
			this.url1 = data.result1;
          if (this.support.datauri || this.uploaded) {
	    this.uploaded = false;
            this.cropDone();
          } else {
            this.uploaded = true;
            this.$avatarSrc.val(this.url);
            this.startCropper();
          }

          //this.$avatarInput.val('');
        } else if (data.message) {
          this.alert(data.message);
        }
      } else {
        this.alert('Failed to response');
      }
    },

    submitFail: function (msg) {
      this.alert(msg);
    },

    submitEnd: function () {
	this.$loading.css("top",0);
      this.$loading.fadeOut();
    },

    cropDone: function () {
		this.$avatarForm.get(0).reset();
		//this.$avatar.attr('src', this.url);
           
		//$("#crop_result").append("<img title='Click to place the location and size on the background image' src='"+this.url+"' id='"+RandomID +"' style='height:75px;width:75px;margin:10px 10px;'/>");

		this.stopCropper();
		this.$avatarModal.modal('hide');
		$("#myCanvas").attr("texsrc",this.url);
		$("#myCanvas").attr("imgsrc",this.url1);
		$("#Furtherimg").attr("src",this.url);
		var imageaa =new Image();
		imageaa.src = this.url;
		imageaa.onload = function(){
			document.getElementById("myCanvas").height = imageaa.height;
			document.getElementById("myCanvas").width = imageaa.width;
			
			$("#myCanvas").attr("style","position:absolute;left:"+(300-imageaa.width/2)+"px; top:0px;background: transparent;");
			
			
			$("#Furtherimg").attr("style","position:absolute; left:"+(300-imageaa.width/2)+"px; top:0px;");
			$("#row1").attr("style","height:"+(100+imageaa.height)+"px");	
			$("#Furtherclear").attr("style","position:absolute;left:200px;top:"+(20+imageaa.height)+"px");
			$("#Furtherpaint").attr("style","position:absolute;left:400px;top:"+(20+imageaa.height)+"px");
		}
		
		
	},
    alert: function (msg) {
      var $alert = [
            '<div class="alert alert-danger avater-alert">',
              '<button type="button" class="close" data-dismiss="alert">&times;</button>',
              msg,
            '</div>'
          ].join('');

      this.$avatarUpload.after($alert);
    },
  };

  $(function () {
    return new CropAvatar($('#crop-avatar'));
  });
  $(function () {
      return new CropAvatar($('#crop-avatar1'));
  });

});
