.section .data

.global tim_image
.type tim_image, @object

.global grass_64
.type grass_64, @object

tim_image:
	.incbin "texture64.tim"

grass_64:
	.incbin "grass64.tim"
