from django.contrib import admin
from django.db import models
from .models import Post, Comment
from django.forms import TextInput, Textarea

class CustomModelAdmin(admin.ModelAdmin):
    formfield_overrides = {
        models.CharField: {'widget': TextInput(attrs={'size':1000})},
        models.TextField: {'widget': Textarea(attrs={'rows':1000,
                                            'cols':1000,
                                            'style':'resize:none;'})},
    }

admin.site.register(Comment, CustomModelAdmin)

# Register your models here.
admin.site.register(Post)