from django import forms
from .models import Comment

class CommentCreationForm(forms.ModelForm):
  # Type of field used in rendered form
  comment = forms.CharField(label="", help_text="", widget=forms.Textarea(attrs={'rows':5,'cols':20, 'style':'resize:none;'}))

  class Meta:
    # Model to save comment as
    model = Comment

    # Fields to display in the rendered form
    fields = ('comment',)

class CommentUpdateForm(forms.ModelForm):
  comment = forms.CharField(label="", help_text="", widget=forms.Textarea(attrs={'rows':5,'cols':20, 'style':'resize:none;'}))

  class Meta:
    model = Comment
    fields = ('comment',)
  
class CommentDeleteForm(forms.ModelForm):
  class Meta:
    model = Comment
    fields = []

