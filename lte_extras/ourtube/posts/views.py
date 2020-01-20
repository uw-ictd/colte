from django.shortcuts import render, get_object_or_404, redirect
from django.contrib.auth.mixins import (
  LoginRequiredMixin,
  UserPassesTestMixin
)
from django.contrib.auth import get_user_model 
from django.views.generic.list import MultipleObjectMixin
from django.views.generic.edit import FormMixin
from django.contrib.auth.decorators import login_required
from django.http import HttpResponseForbidden
from django.views.generic import (
  ListView, 
  DetailView, 
  CreateView,
  UpdateView,
  DeleteView,
)
from django.contrib import messages
from django.urls import reverse
from .models import Post, Comment
from .forms import CommentCreationForm, CommentDeleteForm, CommentUpdateForm


##########################
# Post Views
##########################

# View for listing multiple posts
class PostListView(ListView):
  model = Post
  
  # This is what it looks for
  # <app>/<model>_<viewtype>.html
  # without below line
  template_name = 'posts/post_home.html'

  # Find object to iterate over
  context_object_name = 'posts'

  # Order of the objects to list
  ordering = ['-date_posted']

  # Determines the number of posts that are displayed on a given page
  # Activates pagination
  paginate_by = 5

# Same as above, but narrowed by user
class UserPostListView(ListView):
  model = Post

  template_name = 'posts/user_posts.html'
  context_object_name = 'posts'
  paginate_by = 5

  # Narrow the query set for displaying only post from a given user
  def get_queryset(self):
    user = get_object_or_404(get_user_model(), username=self.kwargs.get('username'))
    return Post.objects.filter(author=user).order_by('-date_posted')

# View to handle displaying a single post 
# Uses MultipleObjectMixin to allow pagination and FormMixin for forms
class PostDetailView(FormMixin, MultipleObjectMixin, DetailView):
  model = Post
  template_name = 'posts/post_detail.html'
  paginate_by = 5
  form_class = CommentCreationForm # Creation is the default

  # Allows multiple models
  def get_context_data(self, **kwargs):
    # Set list to paginate by (would have defaulted to list of posts)
    object_list = Comment.objects.filter(post=self.get_object()).order_by('-date_posted')

    context = super(PostDetailView, self).get_context_data(object_list=object_list, **kwargs)

    # Create form instances and provide contex for html
    context['comment_form'] = self.get_form(CommentCreationForm)
    context['update_form'] = self.get_form(CommentUpdateForm)
    context['delete_form'] = self.get_form(CommentDeleteForm)
    return context

  def get_success_url(self):
    return reverse('post-detail', kwargs={'pk': self.object.pk})
  
  def post(self, request, *args, **kwargs):
    if not request.user.is_authenticated:
      return HttpResponseForbidden()

    self.object = self.get_object() # Needed

    # Create form submitted
    if 'create' in request.POST:
      comment_form = self.get_form(CommentCreationForm)

      if comment_form.is_valid():
        print("Create form submitted")
        # Save without commit give a comment instance
        comment = comment_form.save(commit=False)

        # Tie comment to post it was created on and author
        comment.post = Post.objects.get(pk=kwargs['pk'])
        comment.author = request.user
        messages.success(request, 'Comment Created')
        comment.save()
        return self.form_valid(comment_form)
      else:
        print("Invalid create form")
        return self.form_invalid(comment_form)

    # Update form submitted
    elif 'update' in request.POST:
      # The comment id should have been passed as a key-value pair
      comment_id = request.POST.get('update')
      instance = Comment.objects.get(id=comment_id)
      update_form = CommentUpdateForm(instance=instance, data=request.POST or None)

      if update_form.is_valid():
        print("Update form Submitted")
        update_form.save()
        return self.form_valid(update_form)
      else:
        print("Invalid update form")
        return self.form_valid(update_form)

    # Delete form submitted
    elif 'delete' in request.POST:
      comment_id = request.POST.get('delete')
      instance = Comment.objects.get(id=comment_id)
      delete_form = CommentDeleteForm(instance=instance, data=request.POST or None)

      if delete_form.is_valid():
        print("Delete form submitted")
        instance.delete()
        return self.form_valid(delete_form)
      else:
        print("Invalid delete form")
        return self.form_invalid(delete_form)
    
    # No (known) form submitted
    else:
      print("No matching forms")
      return HttpResponseForbidden()  

class PostCreateView(LoginRequiredMixin, CreateView):
  model = Post
  fields = ['title', 'content', 'video_thumbnail', 'video']

  # Assign the current user as the author
  def form_valid(self, form):
    form.instance.author = self.request.user
    return super().form_valid(form)

class PostUpdateView(LoginRequiredMixin, UserPassesTestMixin, UpdateView):
  model = Post
  fields = ['title', 'content']

  # Assign the current user as the author
  def form_valid(self, form):
    form.instance.author = self.request.user
    return super().form_valid(form)

  # This prevents other users from editing posts that aren't theirs
  def test_func(self):
    post = self.get_object()
    if self.request.user == post.author:
      return True
    return False

class PostDeleteView(LoginRequiredMixin, UserPassesTestMixin, DeleteView):
  model = Post
  success_url = "/" # Where to go after delete succeeds

  def test_func(self):
    post = self.get_object()
    if self.request.user == post.author:
      return True
    return False
