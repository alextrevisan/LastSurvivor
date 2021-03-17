.section .data

.global parchment_tim
.type parchment_tim, @object

.global start_tim
.type start_tim, @object

parchment_tim:
	.incbin "textures/PARCHMENT.TIM"

start_tim:
	.incbin "textures/START.TIM"
	