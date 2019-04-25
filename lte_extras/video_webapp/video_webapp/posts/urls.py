from django.urls import path
from .views import (
  PostListView, 
  PostDetailView,
  PostCreateView,
  PostUpdateView,
  PostDeleteView,
  UserPostListView,
)
from . import views

# URL patterns for each type of page
# <...> indicates variable for URL, pk is the key for the post
urlpatterns = [
    path('', PostListView.as_view(), name="posts-home"),
    path('user/<str:username>', UserPostListView.as_view(), name="user-posts"),
    path('<int:pk>/', PostDetailView.as_view(), name="post-detail"),
    path('new/', PostCreateView.as_view(), name="post-create"),
    path('<int:pk>/update/', PostUpdateView.as_view(), name="post-update"),
    path('<int:pk>/delete/', PostDeleteView.as_view(), name="post-delete"),
]
