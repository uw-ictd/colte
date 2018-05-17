from django.conf.urls import url
from polls import views

urlpatterns = [
	# url(r'^$', views.index, name='index'),
	url(r'^$', views.index, name='index'),
	# url('index.html', views.index, name='index'),
	url(r'^(?P<person_id>[0-9]+)/$', views.detail, name='detail'),
	url('register/', views.register, name='register'),
]

