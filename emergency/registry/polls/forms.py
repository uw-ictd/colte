from django import forms

class AddPersonForm(forms.Form):
	name_box = forms.CharField(max_length=50)
	phone_box = forms.CharField(max_length=15)
	loc_box = forms.CharField(max_length=200)