[bits 32]
global ctx_switch

ctx_switch:
    push ebp
    push ebx
    push esi
    push edi

    mov eax, [esp + 20]    ; arg1 = save_esp_ptr
    mov [eax], esp         ; save current stack pointer (after pushes)
    mov esp, [esp + 24]    ; arg2 = new_esp
    pop edi
    pop esi
    pop ebx
    pop ebp
    ret
